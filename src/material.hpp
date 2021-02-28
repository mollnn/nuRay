#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include "vec3.hpp"

struct Material
{
	vec3 diffuse, reflect, refrect, emission;
	double refrect_index;
};

#endif