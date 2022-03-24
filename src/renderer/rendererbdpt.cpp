#include "../renderer/rendererbdpt.h"

Vinfo vInfo(vec3 c, float pf, float pr, const Triangle *obj, vec3 pos, vec3 uv, float bc1, float bc2)
{
    return {c, pf, pr, obj, pos, uv, bc1, bc2};
}

void RendererBDPT::render(const Camera &camera,
                          const std::vector<Triangle> &triangles,
                          QImage &img,
                          Config &config,
                          std::function<void(bool)> callback,
                          std::atomic<int> &con_flag,
                          std::function<void(float)> progress_report,
                          QMutex &framebuffer_mutex,
                          const Envmap *env_map)
{
    int img_width = config.getValueInt("imgw", 0);
    int img_height = config.getValueInt("imgh", 0);
    int SPP = config.getValueInt("spp", 1);

    SamplerStd sampler;

    float film_size = camera.filmSize();
    float img_size = img_width * img_height;

    auto hemisphereSampler = [&](const vec3 &normal) -> vec3
    {
        float r2 = sampler.random() * 0.999f;
        float phi = sampler.random() * 3.14159 * 2;
        float r = sqrt(r2);
        float h = sqrt(1 - r2);

        vec3 ax0 = abs(normal[0]) < 0.6 ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f);
        vec3 ax1 = ax0.cross(normal).normalized();
        vec3 ax2 = normal.cross(ax1).normalized();
        vec3 wi = h * normal + r * cos(phi) * ax1 + r * sin(phi) * ax2;
        return wi;
    };

    auto checkVisibility = [&](vec3 p, vec3 q) -> bool
    {
        vec3 d = q - p;
        vec3 du = d.normalized();
        auto [t, bc1, bc2, obj] = bvh_.intersection(p + du * 1e-3f, du);
        if (t > d.norm() - 1e-2f)
            return true;
        return false;
    };

    auto printPath = [&](const std::vector<Vinfo> &v)
    {
        for (auto i : v)
        {
            std::cout << i.c << " | ";
        }
        std::cout << std::endl;
    };

    int nbuf = config.getValueInt("exbuf", 1);
    typedef std::vector<std::vector<vec3>> Buffer;
    std::vector<std::vector<Buffer>> buf(nbuf, std::vector<Buffer>(nbuf, Buffer(img_height, std::vector<vec3>(img_width))));

    for (int x = 0; x < img_width; x++)
    {
        for (int y = 0; y < img_height; y++)
        {
            for (int sppi = 0; sppi < SPP; sppi++)
            {
                std::vector<Vinfo> light_path, camera_path;
                float prr = config.getValueFloat("prr", 0.8f);
                auto trace = [&](bool is_light_path, std::vector<Vinfo> &path)
                {
                    while (true)
                    {
                        if (sampler.random() > prr)
                            return;

                        auto [c, pf, pr, obj, pos, uv, bc1, bc2] = path.back();
                        vec3 normal = obj != nullptr ? obj->getNormal(bc1, bc2) : 0.0f;
                        vec3 wi, wo, bxdf;
                        float pdf_proj;

                        // sample bxdf
                        if (path.size() == 1)
                        {
                            if (is_light_path)
                            {
                                wo = hemisphereSampler(normal);
                                pdf_proj = 1.0f / 3.14159f;
                                bxdf = 1.0 / 3.14159f;
                            }
                            else
                            {
                                wo = camera.generateRay(x + sampler.random(), y + sampler.random(), img_width, img_height);
                                pdf_proj = 1.0f; // eliminated
                                bxdf = 1.0f;     // eliminated
                            }
                        }
                        else
                        {
                            wi = (path[path.size() - 2].pos - pos).normalized();
                            wo = obj->mat->sampleBxdf(sampler, wi, normal);
                            pdf_proj = obj->mat->pdf(wi, normal, wo) / abs(wo.dot(normal));
                            bxdf = obj->mat->bxdf(wo, normal, wi, uv);
                        }

                        // get intersection
                        auto [hit_t, hit_bc1, hit_bc2, hit_obj] = bvh_.intersection(pos + 1e-2f * wo, wo);
                        if (hit_obj == nullptr)
                        {
                            break;
                        }
                        vec3 hit_pos = pos + wo * hit_t, hit_uv = hit_obj->getTexCoords(hit_bc1, hit_bc2);

                        // push into stack
                        path.push_back(vInfo(c * bxdf / pdf_proj / prr, 1, 1, hit_obj, hit_pos, hit_uv, hit_bc1, hit_bc2));
                        if (hit_obj->mat->isEmission())
                        {
                            break;
                        }
                    }
                };

                // STEP A: GENERATE LIGHT PATH AND TRACE
                auto light_obj = light_sampler_.sampleLight(sampler);
                auto [light_pos, light_bc1, light_bc2] = light_obj->sample(sampler);
                auto light_normal = light_obj->getNormal(light_bc1, light_bc2);
                auto light_int = light_obj->mat->emission(light_normal, light_normal); // * need fix
                auto light_pdf = light_sampler_.p();
                light_path.push_back(vInfo(light_int / light_pdf * 3.14159f, 1, 1, light_obj, light_pos, 0, 0, 0));
                trace(true, light_path);

                // STEP B: GENERATE CAMERA PATH AND TRACE
                camera_path.push_back(vInfo(1.0f / SPP, 1, 1, nullptr, camera.pos, 0, 0, 0));
                trace(false, camera_path);

                // printPath(light_path);
                // printPath(camera_path);
                // std::cout << std::endl;

                // STEP C: CONNECT PATH
                int S = light_path.size(), T = camera_path.size();
                for (int s = 0; s <= S; s++)
                {
                    for (int t = 1; t <= T; t++)
                    {
                        if (s + t < 2)
                            continue;
                        // *: concate path
                        std::vector<Vinfo> full_path;
                        for (int i = 0; i < s; i++)
                            full_path.push_back(light_path[i]);
                        for (int i = 0; i < t; i++)
                            full_path.push_back(camera_path[t - i - 1]);

                        if (full_path.size() > 0)
                            full_path[0].pf = light_pdf;
                        if (full_path.size() > 1)
                        {
                            vec3 d = (full_path[1].pos - full_path[0].pos).normalized();
                            float d2 = (full_path[1].pos - full_path[0].pos).norm2();
                            vec3 normal = light_normal;
                            vec3 new_normal = full_path[1].obj ? full_path[1].obj->getNormal(full_path[1].bc1, full_path[1].bc2) : -d;
                            float g = normal.dot(d) * new_normal.dot(-d) / d2;
                            full_path[1].pf = 1.0f / 3.14159f * g;
                        }
                        if (full_path.size() > 0)
                            full_path[0].pr = 1.0f; // does not matter
                        if (full_path.size() > 1)
                        {
                            vec3 d = (full_path[s + t - 2].pos - full_path[s + t - 1].pos).normalized();
                            vec3 cam_ray_dir = d;
                            float d2 = (full_path[s + t - 2].pos - full_path[s + t - 1].pos).norm2();
                            vec3 normal = cam_ray_dir;
                            vec3 new_normal = full_path[s + t - 2].obj->getNormal(full_path[s + t - 2].bc1, full_path[s + t - 2].bc2);
                            float g = normal.dot(d) * new_normal.dot(-d) / d2;
                            full_path[s + t - 2].pr = 1.0f / (film_size * pow(camera.gaze.dot(cam_ray_dir), 2)) * g;
                        }

                        // *: evaluate probability
                        for (int i = 2; i < s + t - 1; i++)
                        {
                            vec3 wi = (full_path[i - 2].pos - full_path[i - 1].pos).normalized();
                            vec3 normal = full_path[i - 1].obj->getNormal(full_path[i - 1].bc1, full_path[i - 1].bc2);
                            vec3 wo = (full_path[i].pos - full_path[i - 1].pos).normalized();
                            vec3 new_normal = full_path[i].obj->getNormal(full_path[i].bc1, full_path[i].bc2);
                            float g = abs(wo.dot(normal)) * abs(wo.dot(new_normal)) / (full_path[i].pos - full_path[i - 1].pos).norm2();
                            full_path[i].pf = full_path[i - 1].obj->mat->pdf(wi, normal, wo) / abs(wo.dot(normal)) * g * prr;
                        }
                        for (int i = 0; i < s + t - 2; i++)
                        {
                            vec3 wi = (full_path[i + 2].pos - full_path[i + 1].pos).normalized();
                            vec3 normal = full_path[i + 1].obj->getNormal(full_path[i + 1].bc1, full_path[i + 1].bc2);
                            vec3 wo = (full_path[i].pos - full_path[i + 1].pos).normalized();
                            vec3 new_normal = full_path[i].obj->getNormal(full_path[i].bc1, full_path[i].bc2);
                            float g = abs(wo.dot(normal)) * abs(wo.dot(new_normal)) / (full_path[i].pos - full_path[i + 1].pos).norm2();
                            full_path[i].pr = full_path[i + 1].obj->mat->pdf(wi, normal, wo) / abs(wo.dot(normal)) * g * prr;
                        }

                        // for (int i = 0; i < s + t; i++)
                        // {
                        //     std::cout << full_path[i].pf << "\t";
                        // }
                        // std::cout << std::endl;

                        // for (int i = 0; i < s + t; i++)
                        // {
                        //     std::cout << full_path[i].pr << "\t";
                        // }
                        // std::cout << std::endl;

                        // *: evaluate connecting weight
                        // fs = f(xs-2, xs-1, xs),  ft = f(xs-1, xs, xs+1)
                        vec3 fs, ft, c;
                        float g = 1;

                        // evaluate ft and g
                        if (s > 0)
                        {
                            if (t > 1)
                            {
                                ft = full_path[s].obj->mat->bxdf(
                                    (full_path[s + 1].pos - full_path[s].pos).normalized(),
                                    full_path[s].obj->getNormal(full_path[s].bc1, full_path[s].bc2),
                                    (full_path[s - 1].pos - full_path[s].pos).normalized(),
                                    full_path[s].obj->getTexCoords(full_path[s].bc1, full_path[s].bc2));
                                g = checkVisibility(full_path[s - 1].pos, full_path[s].pos);
                                vec3 d = (full_path[s].pos - full_path[s - 1].pos).normalized();
                                g *= full_path[s - 1].obj->getNormal(full_path[s - 1].bc1, full_path[s - 1].bc2).dot(d);
                                g *= full_path[s].obj->getNormal(full_path[s].bc1, full_path[s].bc2).dot(-d);
                                g /= (full_path[s].pos - full_path[s - 1].pos).norm2();
                            }
                            else
                            {
                                // t = 1
                                vec3 d = (full_path[s].pos - full_path[s - 1].pos).normalized();
                                vec3 cam_ray_dir = -d;
                                ft = 1.0f / (film_size / img_size * pow(camera.gaze.dot(cam_ray_dir), 2));
                                ft /= img_size;
                                g = checkVisibility(full_path[s - 1].pos, full_path[s].pos);
                                g *= full_path[s - 1].obj->getNormal(full_path[s - 1].bc1, full_path[s - 1].bc2).dot(d);
                                g /= (full_path[s].pos - full_path[s - 1].pos).norm2();
                            }
                        }

                        // evaluate fs and c
                        if (s == 0)
                        {
                            // t>=2 now
                            if (full_path[0].obj->mat->isEmission())
                            {
                                // camera ray hit light source
                                vec3 normal = full_path[0].obj->getNormal(full_path[0].bc1, full_path[0].bc2);
                                c = full_path[0].obj->mat->emission(normal, normal) * 3.14159f;
                            }
                        }
                        else if (s == 1)
                        {
                            // xs-2 not exist, xs-1 is light source, use light source "bxdf"
                            fs = 1.0f / 3.14159f;
                            c = fs * g * ft;
                        }
                        else
                        {
                            fs = full_path[s - 1].obj->mat->bxdf(
                                (full_path[s].pos - full_path[s - 1].pos).normalized(),
                                full_path[s - 1].obj->getNormal(full_path[s - 1].bc1, full_path[s - 1].bc2),
                                (full_path[s - 2].pos - full_path[s - 1].pos).normalized(),
                                full_path[s - 1].obj->getTexCoords(full_path[s - 1].bc1, full_path[s - 1].bc2));
                            c = fs * g * ft;
                        }

                        // *: add contribution
                        vec3 contrib = (s == 0 ? 1.0f : full_path[s - 1].c) * c * full_path[s].c;
                        float mis_weight = 1;
                        bool USE_MIS = config.getValueInt("mis", 1);
                        if (USE_MIS)
                        {
                            float ratio = 1;
                            for (int i = s - 1; i >= 0; i--)
                            {
                                ratio *= full_path[i].pr / full_path[i].pf;
                                mis_weight += ratio;
                            }
                            ratio = 1;
                            for (int i = s + 1; i < s + t; i++)
                            {
                                ratio *= full_path[i - 1].pf / full_path[i - 1].pr;
                                mis_weight += ratio;
                            }
                            mis_weight = 1.0f / mis_weight;
                        }
                        else
                        {
                            mis_weight = 1.0f / (s + t); // case t=0 does not exist
                        }

                        if (std::isinf(mis_weight) || std::isnan(mis_weight) || mis_weight > 1e9 || mis_weight < 0)
                            mis_weight = 0;

                        // std::cout << "s=" << s << " t=" << t << " w=" << mis_weight << std::endl;

                        auto contribution = contrib * mis_weight;
                        contribution = max(contribution, 0.0f);

                        if (t == 1)
                        {
                            // splat
                            vec3 d = (full_path[s].pos - full_path[s - 1].pos).normalized();
                            vec3 cam_ray_dir = -d;
                            auto [xx, yy] = camera.getCoord(cam_ray_dir, img_width, img_height);
                            if (xx != -1)
                            {
                                buf[0][0][xx][yy] += contribution;
                                if (s < nbuf && t < nbuf)
                                {
                                    buf[s][t][xx][yy] += contribution;
                                }
                            }
                        }
                        else
                        {
                            // contribute to current pixel
                            buf[0][0][x][y] += contribution;
                            if (s < nbuf && t < nbuf)
                            {
                                buf[s][t][x][y] += contribution;
                            }
                        }
                    }
                }
            }
        }
    }

    img = QImage(img_width * nbuf, img_height * nbuf, QImage::Format_RGB888);

    for (int i = 0; i < nbuf; i++)
    {
        for (int j = 0; j < nbuf; j++)
        {
            for (int x = 0; x < img_width; x++)
            {
                for (int y = 0; y < img_height; y++)
                {
                    auto result = (buf[i][j][x][y] * pow(2.0, std::max(0.0, i + j - 2.0))).pow(1.0f / 2.2f) * 255.0f;
                    img.setPixel(x + j * img_width, y + i * img_height, qRgb(std::min(255.0f, std::max(0.0f, result[0])), std::min(255.0f, std::max(0.0f, result[1])), std::min(255.0f, std::max(0.0f, result[2]))));
                }
            }
        }
    }
}
