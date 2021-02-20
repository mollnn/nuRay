render_minipt

希望尽可能简短地去写一个 path tracer （但还是写了半个下午？）

中途把三角形射线求交部分的公式抄错了一个字母，调了半天

写高耦合度的原因是希望能对核心原理部分的实现尽可能做到熟悉

为了省事只写了漫反射和自发光的部分，其实还是挺傻叉的

加入了用打表来加速计算并不精确的 cos 的结构，对性能有大约 `10%` 的提升

博客上只丢核心部分代码，其实其它几个文件也都是贴以前的

调了一个小剧院的场景（就这？），地板+幕布+演员（三个三角形），一个背景光，一个射灯，好像还有点那种感觉

``` cpp
#include <bits/stdc++.h>
using namespace std;

#include "vec3.hpp"
#include "image.hpp"
#include "timer.hpp"

const double pi = acos(-1);
const double eps = 1e-6;

double randf() { return 1.0f * rand() / RAND_MAX; }

struct Material
{
	vec3 diffuse, emission;
};

struct Triangle
{
	vec3 p0, p1, p2;
	Material mat;
	vec3 normal() { return ((p1 - p0).cross(p2 - p0)).unit(); }
	std::pair<double, vec3> intersect(vec3 pos, vec3 dir)
	{
		vec3 tpos = p0, tdir = normal();
		if (fabs(tdir.dot(dir)) < eps)
			return {-1, vec3()};
		if (tdir.dot(dir) < 0)
			tdir = -1 * tdir;
		double x = (tpos - pos).dot(tdir) / (tdir.dot(dir));
		if (x < 0)
			return {-1, vec3()};
		vec3 ph = pos + x * dir;
		double dis = (ph - p0).dot(normal());
		vec3 q0 = (p1 - p0).cross(ph - p0);
		vec3 q1 = (p2 - p1).cross(ph - p1);
		vec3 q2 = (p0 - p2).cross(ph - p2);
		if (q0.dot(q1) > 0 && q1.dot(q2) > 0 && q2.dot(q0) > 0)
		{
			return {x, ph};
		}
		else
			return {-1, vec3()};
	}
};

std::tuple<double, vec3, Triangle *> intersect(std::vector<Triangle> &triangles, vec3 pos, vec3 dir)
{
	double hitdis = 2e18;
	vec3 hitpos;
	Triangle *hitobj = NULL;
	for (auto &triangle : triangles)
	{
		auto [thdis, thpos] = triangle.intersect(pos, dir);
		if (thdis > 0 && thdis < hitdis)
		{
			hitdis = thdis;
			hitpos = thpos;
			hitobj = &triangle;
		}
	}
	if (hitdis > 1e18)
		return {-1, hitpos, hitobj};
	return {hitdis, hitpos, hitobj};
}

namespace fastmath
{
	const int siz_cos_l = 1e4;
	const double lim_cos_l = 2 * pi;
	const double step_cos_l = lim_cos_l / siz_cos_l;

	vector<double> mem_cos_l(siz_cos_l + 2);

	double get_cos_l(double x)
	{
		if (x < 0 || x > 2 * pi)
			x = fmod(x, 2 * pi);
		int idx = (x + 1e-7) / step_cos_l;
		return mem_cos_l[idx];
	}

	double get_cos_l_(double x)
	{
		if (x < 0 || x > 2 * pi)
			x = fmod(x, 2 * pi);
		int idx1k = 1e3 * (x + 1e-7) / step_cos_l;
		int idx = idx1k / 1000;
		int offset = idx1k - idx * 1000;
		return (mem_cos_l[idx] * offset + mem_cos_l[idx + 1] * (1000 - offset)) / 1000;
	}

	void presolve()
	{
		cout << "Math Presolving..." << endl;
		for (int i = 0; i <= siz_cos_l; i++)
		{
			mem_cos_l[i] = cos(lim_cos_l / siz_cos_l * i);
		}
		cout << "Math Finish!" << endl;
	}
};

vec3 PathTrace(vec3 raypos, vec3 raydir, int depth, std::vector<Triangle> &triangles)
{
	if (depth > 6)
		return {0, 0, 0};
	auto [hitdis, hitpos, hitobj] = intersect(triangles, raypos, raydir);
	if (hitdis < 0)
		return {0, 0, 0};
	double r2 = randf();
	double phi = randf() * 2 * pi;
	double sqr2 = sqrt(r2);
	// double cosphi = cos(phi);
	double cosphi = fastmath::get_cos_l(phi);
	double sinphi = sqrt(1 - cosphi * cosphi) * (phi < pi ? 1 : -1);
	double du = sqr2 * cosphi;
	double dv = sqr2 * sinphi;
	double dw = sqrt(1 - r2);
	vec3 normal = hitobj->normal();
	if (raydir.dot(normal) > 0)
		normal = -1 * normal;
	vec3 ew = normal;
	vec3 eu = ((vec3){randf(), randf(), randf()}).unit().cross(normal);
	vec3 ev = eu.cross(ew);
	vec3 difdir = du * eu + dv * ev + dw * ew;
	return hitobj->mat.emission + hitobj->mat.diffuse * PathTrace(hitpos + eps * difdir, difdir, depth + 1, triangles);
}

void render(int img_siz_x, int img_siz_y, int spp, vec3 cam_pos, vec3 cam_dir, vec3 cam_top, double focal, double near_clip,
			Image &image, vector<Triangle> &scene)
{
	Timer timer, t_timer;

	double fov = 2 * atan(36 / 2 / focal);
	double fp_siz_x = 2 * tan(fov / 2);
	double fp_siz_y = fp_siz_x * img_siz_y / img_siz_x;
	vec3 fp_e_y = cam_top;
	vec3 fp_e_x = cam_dir.cross(fp_e_y);

	for (int img_x = 0; img_x < img_siz_x; img_x++)
	{
		if (t_timer.Current() > 1)
		{
			cout << "Rendering... " << fixed << setprecision(2) << (1.0 * img_x / img_siz_x * 100) << "%" << endl;
			t_timer.Start();
		}
		for (int img_y = 0; img_y < img_siz_y; img_y++)
		{
			for (int t = 0; t < spp; t++)
			{
				double x = img_x + 0.5 * (-1 + 2 * randf());
				double y = img_y + 0.5 * (-1 + 2 * randf());
				vec3 focus_pos = cam_pos + (cam_dir + (x / img_siz_x - 0.5) * fp_siz_x * fp_e_x + (y / img_siz_y - 0.5) * fp_siz_y * fp_e_y) * near_clip;
				vec3 raypos = focus_pos;
				vec3 raydir = (focus_pos - cam_pos).unit();
				vec3 radiance = PathTrace(raypos, raydir, 0, scene);

				image.Add(img_x, img_y, radiance / (1.0 * spp));
			}
		}
	}
	double rendertime = timer.Current();
	cout << "Finish!  Time cost: " << fixed << setprecision(2) << rendertime << "s" << endl;
}

int main(int argc, char *argv[])
{
	fastmath::presolve();

	std::vector<Triangle> scene;

	scene.push_back({{-1, 0, 0}, {1, 0, 0}, {0, 0, 2}, {{0.8, 0.8, 0.8}, {0, 0, 0}}});
	scene.push_back({{-2, -7, 0}, {2, -7, 0}, {0, -7, 2}, {{0, 0, 0}, {4, 4, 4}}});
	scene.push_back({{-1e2, 1e2, 0}, {1e2, 1e2, 0}, {0, -1e2, 0}, {{0.3, 0.3, 0.3}, {0, 0, 0}}});
	scene.push_back({{-10, -10, 3}, {10, -10, 3}, {0, -5, 5}, {{0, 0, 0}, {4, 2.5, 0}}});
	scene.push_back({{-1e2, 2, 0}, {1e2, 2, 0}, {0, 2, 1e3}, {{0.5, 0.5, 0.5}, {0, 0, 0}}});

	int img_siz_x = 1024;
	double img_aspect = 2.39;
	int img_siz_y = img_siz_x / img_aspect;
	int spp = 1024;
	vec3 cam_dir = (vec3){0.8, 1, 0.14}.unit();
	vec3 cam_pos = {-3, -5, 0.5};
	vec3 cam_top = {0, 0, 1};
	double focal = 24;
	double near_clip = 0.1;

	Image image(img_siz_x, img_siz_y);

	render(img_siz_x, img_siz_y, spp, cam_pos, cam_dir, cam_top, focal, near_clip, image, scene);

	image.WriteToTGA("output.tga");
}
```