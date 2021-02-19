#include <bits/stdc++.h>
using namespace std;

const float pi = acos(-1);
const float eps = 1e-6;

float randf() { return 1.0f * rand() / RAND_MAX; }

struct vec3
{
	float x, y, z;
	vec3 operator+(const vec3 &rhs) { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
	vec3 operator-(const vec3 &rhs) { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
	vec3 operator*(const vec3 &rhs) { return {x * rhs.x, y * rhs.y, z * rhs.z}; }
	vec3 operator*(float rhs) { return {x * rhs, y * rhs, z * rhs}; }
	vec3 operator/(float rhs) { return {x / rhs, y / rhs, z / rhs}; }
	float dot(const vec3 &rhs) { return x * rhs.x + y * rhs.y + z * rhs.z; }
	vec3 cross(const vec3 &rhs) { return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x}; }
	float norm2() { return x * x + y * y + z * z; }
	float norm() { return sqrt(norm2()); }
	vec3 unit() { return (*this) / norm(); }
};
vec3 operator*(float lhs, const vec3 &rhs) { return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z}; }

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
		float x = (tpos - pos).dot(tdir) / (dir.dot(tdir));
		if (x < 0)
			return {-1, vec3()};
		return {x, tpos + x * tdir};
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
		if (thdis < hitdis)
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
	if (depth > 5)
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
	
}