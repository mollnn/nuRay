#include "rendererbdpt.h"
#include "texture.h"
#include "lightsampler.h"
#include "bvh.h"
#include <QTime>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include "samplerstd.h"

// ! THIS IMPLEMENTATION IS JUST FOR TEST. IT'S NOT CORRECT.

bool RendererBDPT::visibility(vec3 p, vec3 q, const std::vector<Triangle> &triangles)
{
    vec3 d = q - p;
    p += d * 1e-6f;
    q -= d * 1e-6f;
    return std::get<0>(intersect(p, (q - p).normalized(), triangles, bvh_)) > 0.9999f * d.norm();
}

vec3 RendererBDPT::connect(const std::vector<BDPTVertexInfo> &light_path, const std::vector<BDPTVertexInfo> &eye_path, const std::vector<Triangle> &triangles)
{
    vec3 ans;
    for (int s = 0; s < light_path.size(); s++)
    {
        for (int t = 0; t < eye_path.size(); t++)
        {
            if (t < 2)
                continue;
            if (s == 0)
                continue; // handled by return value of trace()
            // TODO: MIS weight
            const Triangle *py = light_path[s].primitive;
            const Triangle *pz = eye_path[t].primitive;
            if (!py || !pz)
                continue;
            vec3 y1 = light_path[s].pos, y2 = light_path[s - 1].pos;
            vec3 z1 = eye_path[t].pos, z2 = eye_path[t - 1].pos;
            vec3 ywi = (y2 - y1).normalized(), ywo = (z1 - y1).normalized();
            vec3 zwi = (y1 - z1).normalized(), zwo = (z2 - z1).normalized();
            float yb1 = light_path[s].b1, yb2 = light_path[s].b2;
            float zb1 = eye_path[s].b1, zb2 = eye_path[s].b2;
            float g = visibility(y1, z1, triangles) * abs(ywi.dot(py->getNormal(yb1, yb2)) * zwi.dot(pz->getNormal(zb1, zb2))) / (z1 - y1).norm2();

            vec3 fs = py->mat->bxdf((z1 - y1).normalized(), py->getNormal(yb1, yb2), (y2 - y1).normalized(), py->getTexCoords(yb1, yb2));
            vec3 ft = pz->mat->bxdf((z2 - z1).normalized(), pz->getNormal(zb1, zb2), (y1 - z1).normalized(), pz->getTexCoords(zb1, zb2));
            vec3 c = fs * g * ft;
            ans += light_path[s].alpha * c * eye_path[t].alpha;
        }
    }
    return ans;
}

vec3 RendererBDPT::trace(std::vector<BDPTVertexInfo> &record, const vec3 &initial, Sampler &sampler, const vec3 &orig0, const vec3 &dir0, const std::vector<Triangle> &triangles, LightSampler &light_sampler_, BVH &bvh_, const Texture *env_map)
{
    vec3 value = initial;

    vec3 o = orig0, d = dir0;
    while (true)
    {
        auto [t, b1, b2, hit_obj] = intersect(o, d, triangles, bvh_);

        if (hit_obj == nullptr)
        {
            if (env_map == nullptr)
            {
                value *= 0.0f;
                break;
            }
            else
            {
                float v = 1 - acos(d[1]) / 3.14159;
                float u = atan2(d[2], d[0]) / 2 / 3.14159 + 0.5;
                value *= env_map->pixelUV(u, v);
                break;
            }
        }

        vec3 texcoords = hit_obj->getTexCoords(b1, b2);
        vec3 hit_pos = o + d * t;
        float u = texcoords[0], v = texcoords[1];

        if (hit_obj != nullptr)
        {
            BDPTVertexInfo vi;
            vi.alpha = value;
            vi.pos = hit_pos;
            vi.primitive = hit_obj;
            vi.b1 = u;
            vi.b2 = v;
            record.push_back(vi);
        }

        vec3 wo = -d;
        vec3 normal = hit_obj->getNormal(b1, b2);

        if (hit_obj->mat->isEmission())
        {
            value *= hit_obj->mat->emission(wo, normal);
            break;
        }

        if (normal.dot(d) > 0 && hit_obj->mat->isTransmission() == false)
        {
            value *= 0.0f;
            break;
        }

        vec3 result;

        // Round Robin
        float prr = 0.8;
        if (sampler.random() > prr)
        {
            value *= 0.0f;
            break;
        }

        // sample bxdf
        vec3 wi = hit_obj->mat->sampleBxdf(sampler, wo, normal);
        float pdf = hit_obj->mat->pdf(wo, normal, wi);
        vec3 brdf = hit_obj->mat->bxdf(wo, normal, wi, texcoords);
        value *= abs(wi.dot(normal)) * brdf / pdf / prr;
        d = wi;
        o = hit_pos + d * 1e-3f;
    }

    return value;
}

void RendererBDPT::render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img, int SPP, int img_width, int img_height, std::function<void(bool)> requestDisplayUpdate, std::atomic<int> &con_flag, std::function<void(float)> progress_report, QMutex &framebuffer_mutex, const Texture *env_map)
{
    SamplerStd sampler;

    requestDisplayUpdate(false);
    framebuffer_mutex.lock();
    img = QImage(QSize(img_width, img_height), QImage::Format_RGB888);
    img.fill(Qt::black);
    framebuffer_mutex.unlock();

    QTime time;
    time.start();
    auto time_last = time.elapsed();

    std::cout << "Rendering... " << std::endl;

    std::atomic<int> pxc = 0;

    auto requestProgressUpdate = [&]()
    {
        float progress = pxc * 1.0f / img_height / img_width;
        progress_report(progress * 100);
        if (time.elapsed() - time_last > 1000)
        {
            std::cout << std::fixed << std::setprecision(2) << "Rendering... " << progress * 100 << "%"
                      << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
            time_last = time.elapsed();
        }
    };

    int block_size = 8;
    std::vector<std::pair<int, int>> task_queue;
    std::mutex task_mutex;

    std::atomic request_disp_update = 0;

    for (int y = 0; y < img_height; y += block_size)
    {
        for (int x = 0; x < img_width; x += block_size)
        {
            task_queue.push_back({x, y});
        }
    }

    std::random_shuffle(task_queue.begin(), task_queue.end());

    auto hemisphereSampler = [&](const vec3 &normal) -> vec3
    {
        float r2 = sampler.random() * 0.999f;
        float phi = sampler.random() * 3.14159 * 2;
        float r = sqrt(r2);
        float h = sqrt(1 - r2);

        vec3 ax0 = abs(normal[0]) < 0.8 ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f);
        vec3 ax1 = ax0.cross(normal).normalized();
        vec3 ax2 = normal.cross(ax1).normalized();
        vec3 wi = h * normal + r * cos(phi) * ax1 + r * sin(phi) * ax2;
        return wi;
    };

    auto workerFunc = [&]()
    {
        while (true)
        {
            if (con_flag == 0)
            {
                break;
            }

            task_mutex.lock();
            if (task_queue.size() == 0)
            {
                task_mutex.unlock();
                break;
            }
            auto [x0, y0] = task_queue.back();
            task_queue.pop_back();
            task_mutex.unlock();

            for (int y = y0; y < img_height && y < y0 + block_size; y++)
            {
                for (int x = x0; x < img_width && x < x0 + block_size; x++)
                {
                    vec3 result;
                    for (int i = 0; i < SPP; i++)
                    {
                        vec3 ans;
                        vec3 eye_ray_dir = camera.generateRay(x + sampler.random(), y + sampler.random(), img_width, img_height);
                        std::vector<BDPTVertexInfo> light_path;
                        auto light_obj = light_sampler_.sampleLight(sampler);
                        auto [light_pos, light_b1, light_b2] = light_obj->sample(sampler);
                        auto light_dir = hemisphereSampler(light_obj->getNormal(light_b1, light_b2));
                        auto light_int = light_obj->mat->emission(light_dir, light_obj->getNormal(light_b1, light_b2)) / light_sampler_.p();
                        light_path.push_back({nullptr, 0, 0, 0, 1});
                        light_path.push_back({light_obj, light_pos, 0, 0, light_int}); // u,v is temply replaced with 0,0

                        trace(light_path, light_int * 1.0f, sampler, light_pos, light_dir, triangles, light_sampler_, bvh_, env_map);

                        std::vector<BDPTVertexInfo> eye_path;
                        eye_path.push_back({nullptr, 0, 0, 0, 1});
                        eye_path.push_back({nullptr, camera.pos, 0, 0, 1});
                        ans += trace(eye_path, 1.0f, sampler, camera.pos, eye_ray_dir, triangles, light_sampler_, bvh_, env_map);
                        ans += connect(light_path, eye_path, triangles);
                        result += max(0.0f, ans);
                    }
                    result /= SPP;
                    // Gamma correction
                    result = result.pow(1.0 / 2.2);
                    result *= 255.0f;
                    framebuffer_mutex.lock();
                    img.setPixel(x, y, qRgb(std::min(255.0f, std::max(0.0f, result[0])), std::min(255.0f, std::max(0.0f, result[1])), std::min(255.0f, std::max(0.0f, result[2]))));
                    framebuffer_mutex.unlock();
                }
            }
            pxc += block_size * block_size;
            requestProgressUpdate();
            request_disp_update = 1;
        }
    };

    int num_threads = 8;
    std::vector<std::thread> ths;
    for (int i = 0; i < num_threads; i++)
    {
        ths.push_back(std::thread(workerFunc));
    }

    while (pxc < img_width * img_height)
    {
        if (request_disp_update == 1)
        {
            request_disp_update = 0;
            requestDisplayUpdate(false);
        }
        // Just delay
        QEventLoop loop;
        QTimer timer;
        timer.setInterval(100);
        timer.start();
        QObject::connect(&timer, &QTimer::timeout, [&]()
                         { loop.quit(); });
        loop.exec();
        if (con_flag == 0)
        {
            break;
        }
    }

    for (auto &i : ths)
    {
        i.join();
    }

    if (con_flag == 0)
    {
        std::cout << "Cancelled" << std::endl;
        return;
    }

    std::cout << std::fixed << std::setprecision(2) << "Rendering... " << 100.0 << "%"
              << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
    con_flag = 0;
    requestDisplayUpdate(true);
}
