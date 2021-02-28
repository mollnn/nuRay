#ifndef _SPHERICAL_HPP_
#define _SPHERICAL_HPP_

#include "vec3.hpp"
#include "material.hpp"
#include "fastmath.hpp"

class Spherical
{
public:
	vec3 c;
	double r;
	Material mat;
	vec3 math_normal(vec3 p) const { return (p - c).unit(); }
	vec3 normal(vec3 p) const { return (p - c).unit(); }
	std::pair<double, vec3> intersect(vec3 pos, vec3 dir) const
	{
		vec3 direction = dir;
		vec3 center_to_origin = pos - c;
		double radius = r;

		double equation_coefficient_a = direction.dot(direction);
		double equation_coefficient_b = 2 * center_to_origin.dot(direction);
		double equation_coefficient_c = center_to_origin.dot(center_to_origin) - radius * radius;

		double equation_delta = sqrt(equation_coefficient_b * equation_coefficient_b - 4 * equation_coefficient_a * equation_coefficient_c);
		double equation_root1 = (-equation_coefficient_b - equation_delta) / 2;
		double equation_root2 = (-equation_coefficient_b + equation_delta) / 2;

		double intersection_distance = equation_root1 >= 0 ? equation_root1 : equation_root2;

		if (intersection_distance > 0)
		{
			vec3 hitpos = pos + intersection_distance * dir;
			return {intersection_distance, hitpos};
		}
		else
		{
			return {-1, {0, 0, 0}};
		}
	}
};

std::tuple<double, vec3, const Spherical *> Intersect(const std::vector<Spherical> &sphericals, vec3 pos, vec3 dir)
{
	double hitdis = 2e18;
	vec3 hitpos;
	const Spherical *hitobj = NULL;
	for (auto &spherical : sphericals)
	{
		auto [thdis, thpos] = spherical.intersect(pos, dir);
		if (thdis > 0 && thdis < hitdis)
		{
			hitdis = thdis;
			hitpos = thpos;
			hitobj = &spherical;
		}
	}
	if (hitdis > 1e18)
		return {-1, hitpos, hitobj};
	return {hitdis, hitpos, hitobj};
}


#endif