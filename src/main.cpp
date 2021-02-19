#include <bits/stdc++.h>
using namespace std;

#include "vec3.hpp"
#include "image.hpp"

const float pi = acos(-1);
const float eps = 1e-6;

float randf() { return 1.0f * rand() / RAND_MAX; }

struct Material
{
	vec3 diffuse, emission;
};

struct Triangle
{
	vec3 p0, p1, p2;
	Material mat;
	vec3 normal() { return ((p1 - p0).cross(p2 - p0)).unit(); }
	std::pair<float, vec3> intersect(vec3 pos, vec3 dir)
	{
		vec3 tpos = p0, tdir = normal();
		if (fabs(tdir.dot(dir)) < eps)
			return {-1, vec3()};
		if (tdir.dot(dir) < 0)
			tdir = -1 * tdir;
		float x = (tpos - pos).dot(tdir) / (tdir.dot(dir));
		if (x < 0)
			return {-1, vec3()};
		vec3 ph = pos + x * dir;
		float dis = (ph - p0).dot(normal());
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

std::tuple<float, vec3, Triangle *> intersect(std::vector<Triangle> &triangles, vec3 pos, vec3 dir)
{
	float hitdis = 2e18;
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

vec3 PathTrace(vec3 raypos, vec3 raydir, int depth, std::vector<Triangle> &triangles)
{
	if (depth > 6)
		return {0, 0, 0};
	auto [hitdis, hitpos, hitobj] = intersect(triangles, raypos, raydir);
	if (hitdis < 0)
		return {0, 0, 0};
	float r2 = randf();
	float phi = randf() * 2 * pi;
	float du = sqrt(r2) * cos(phi);
	float dv = sqrt(r2) * sin(phi);
	float dw = sqrt(1 - r2);
	vec3 normal = hitobj->normal();
	if (raydir.dot(normal) > 0)
		normal = -1 * normal;
	vec3 ew = normal;
	vec3 eu = ((vec3){randf(), randf(), randf()}).unit().cross(normal);
	vec3 ev = eu.cross(ew);
	vec3 difdir = du * eu + dv * ev + dw * ew;
	return hitobj->mat.emission + hitobj->mat.diffuse * PathTrace(hitpos + eps * difdir, difdir, depth + 1, triangles);
}

int main(int argc, char *argv[])
{
	std::vector<Triangle> scene;
	scene.push_back({{-1, 0, 0}, {1, 0, 0}, {0, 0, 2}, {{0.8, 0.8, 0.8}, {0, 0, 0}}});
	scene.push_back({{-2, -7, 0}, {2, -7, 0}, {0, -7, 2}, {{0, 0, 0}, {4, 4, 4}}});
	scene.push_back({{-1e2, 1e2, 0}, {1e2, 1e2, 0}, {0, -1e2, 0}, {{0.3, 0.3, 0.3}, {0, 0, 0}}});
	scene.push_back({{-10, -10, 3}, {10, -10, 3}, {0, -5, 5}, {{0, 0, 0}, {4, 2.5, 0}}});
	scene.push_back({{-1e2, 2, 0}, {1e2, 2, 0}, {0, 2, 1e3}, {{0.5, 0.5, 0.5}, {0, 0, 0}}});
	int img_siz_x = 1024;
	float img_aspect = 2.39;
	int img_siz_y = img_siz_x / img_aspect;
	int spp = 8;
	vec3 cam_dir = (vec3){0.5, 1, 0}.unit();
	vec3 cam_pos = {-2, -5, 1};
	vec3 cam_top = {0, 0, 1};
	float focal = 24;
	float fov = 2 * atan(36 / 2 / focal);
	float fp_siz_x = 2 * tan(fov / 2);
	float fp_siz_y = fp_siz_x * img_siz_y / img_siz_x;
	float near_clip = 0.1;
	vec3 fp_e_y = cam_top;
	vec3 fp_e_x = cam_dir.cross(fp_e_y);

	Image image(img_siz_x, img_siz_y);

	for (int img_x = 0; img_x < img_siz_x; img_x++)
	{
		if (img_x % 16 == 0)
			cout << "Rendering... " << fixed << setprecision(2) << (1.0 * img_x / img_siz_x * 100) << "%" << endl;
		for (int img_y = 0; img_y < img_siz_y; img_y++)
		{
			for (int t = 0; t < spp; t++)
			{
				float x = img_x + 0.5 * (-1 + 2 * randf());
				float y = img_y + 0.5 * (-1 + 2 * randf());
				vec3 focus_pos = cam_pos + (cam_dir + (x / img_siz_x - 0.5) * fp_siz_x * fp_e_x + (y / img_siz_y - 0.5) * fp_siz_y * fp_e_y) * near_clip;
				vec3 raypos = focus_pos;
				vec3 raydir = (focus_pos - cam_pos).unit();
				vec3 radiance = PathTrace(raypos, raydir, 0, scene);

				image.Add(img_x, img_y, radiance / (1.0 * spp));
			}
		}
	}

	image.WriteToTGA("output.tga");
}