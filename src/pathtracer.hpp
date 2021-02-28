#ifndef _PATH_TRACER_HPP_
#define _PATH_TRACER_HPP_

#include <bits/stdc++.h>
using namespace std;

#include "vec3.hpp"
#include "timer.hpp"
#include "fastmath.hpp"
#include "random.hpp"
#include "material.hpp"
#include "triangle.hpp"
#include "spherical.hpp"
#include "scene.hpp"

class PathTracer
{
    public:
    vec3 PathTrace(vec3 raypos, vec3 raydir, int depth, const std::vector<Triangle> &triangles, const std::vector<Spherical> &sphericals)
    {
        if (depth > 10)
            return {0, 0, 0};

        const Material *p_mat = NULL;
        vec3 normal_outward;

        auto [hitdis, hitpos, hitobj] = Intersect(triangles, raypos, raydir);
        if (hitdis > 0)
        {
            p_mat = &hitobj->mat;
            normal_outward = hitobj->normal();
        }

        auto [hitdis2, hitpos2, hitobj2] = Intersect(sphericals, raypos, raydir);
        if (hitdis2 > 0 && hitdis2 < hitdis)
        {
            hitdis = hitdis2;
            hitpos = hitpos2;
            p_mat = &hitobj2->mat;
            normal_outward = hitobj2->normal(hitpos);
        }

        if (hitdis <= 0)
            return {0, 0, 0};

        const Material &material = *p_mat;                            // 命中物体的材质
        bool is_into = raydir.dot(normal_outward) < 0;                // 是否在射入内部
        vec3 normal = is_into ? normal_outward : -1 * normal_outward; // 实际光学效应的法线
        double relative_refract_index = is_into ? material.refrect_index : 1.0 / material.refrect_index;

        vec3 in_dir = raydir;                                 // 光线入射方向
        double cos_i = abs(in_dir.dot(normal));               // 入射角 cos
        double sin_i = sqrt(1 - cos_i * cos_i);               // 入射角 sin
        double sin_j = sin_i / relative_refract_index;        // 折射角 sin
        double cos_j = sqrt(1 - sin_j * sin_j);               // 折射角 cos
        vec3 base_dir = (in_dir + cos_i * normal).unit();     // 入射面中表面的切线方向
        vec3 reflect_dir = (in_dir + 2 * cos_i * normal);     // 反射光线方向
        vec3 refrect_dir = sin_j * base_dir - cos_j * normal; // 折射光线方向

        double refrect_index_in = is_into ? 1.0 : material.refrect_index;                                      // 入射介质折射率
        double refrect_index_out = is_into ? material.refrect_index : 1.0;                                     // 折射介质折射率
        double refrect_index_sum = refrect_index_in + refrect_index_out;                                       // 折射率之和
        double refrect_index_delta = refrect_index_in - refrect_index_out;                                     // 折射率之差
        double fresnel_i0 = refrect_index_delta * refrect_index_delta / refrect_index_sum / refrect_index_sum; // 菲涅尔反射光强系数
        double fresnel_x = 1 - cos_i;                                                                          // 菲涅尔反射光强因子

        // 特殊处理全反射情况
        if (sin_j > 1 - 1e-6)
            fresnel_x = 1;
        if (isnanf(fresnel_x))
            fresnel_x = 1;

        double fresnel_reflect_intensity = fresnel_i0 + (1 - fresnel_i0) * pow(fresnel_x, 5); // 菲涅尔反射强度
        double fresnel_refrect_intensity = 1 - fresnel_reflect_intensity;                     // 菲涅尔折射强度

        // 搜索各种光线的概率
        double reflect_probability = material.reflect.avg() + material.refrect.avg() * fresnel_reflect_intensity; // 总镜面反射概率
        double refrect_probability = material.refrect.avg() * fresnel_refrect_intensity;                          // 总折射概率
        double diffuse_probability = material.diffuse.avg();                                                      // 总漫反射概率

        double sum_probability = diffuse_probability + reflect_probability + refrect_probability + eps;
        diffuse_probability /= sum_probability;
        reflect_probability /= sum_probability;
        refrect_probability /= sum_probability;

        // 光线强度
        vec3 reflect_intensity = material.reflect + material.refrect * fresnel_reflect_intensity; // 总镜面反射强度
        vec3 refrect_intensity = material.refrect * fresnel_refrect_intensity;                    // 总折射强度
        vec3 diffuse_intensity = material.diffuse;                                                // 总漫反射强度

        vec3 ans = hitobj->mat.emission;

        double rand_value = randf();
        if (rand_value < diffuse_probability)
        {
            double r2 = randf();
            double phi = randf() * 2 * pi;
            double sqr2 = sqrt(r2);
            double cosphi = fastmath::get_cos_l(phi);
            double sinphi = sqrt(1 - cosphi * cosphi) * (phi < pi ? 1 : -1);
            double du = sqr2 * cosphi;
            double dv = sqr2 * sinphi;
            double dw = sqrt(1 - r2);
            if (raydir.dot(normal) > 0)
                normal = -1 * normal;
            vec3 ew = normal;
            vec3 eu = ((vec3){randf(), randf(), randf()}).unit().cross(normal);
            vec3 ev = eu.cross(ew);
            vec3 difdir = du * eu + dv * ev + dw * ew;
            ans = ans + hitobj->mat.diffuse * PathTrace(hitpos + eps * difdir, difdir, depth + 1, triangles, sphericals);
        }
        else if (rand_value - diffuse_probability < reflect_probability)
        {
            ans = ans + PathTrace(hitpos + eps * reflect_dir, reflect_dir, depth + 1, triangles, sphericals) * reflect_intensity / reflect_probability;
        }
        else if (rand_value - diffuse_probability - reflect_probability < refrect_probability)
        {
            ans = ans + PathTrace(hitpos + eps * refrect_dir, refrect_dir, depth + 1, triangles, sphericals) * refrect_intensity / refrect_probability;
        }
        return ans;
    }
};

#endif