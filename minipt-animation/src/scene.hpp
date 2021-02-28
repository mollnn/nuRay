#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <bits/stdc++.h>
using namespace std;

#include "triangle.hpp"
#include "spherical.hpp"

struct Scene
{
	std::vector<Triangle> triangles;
	std::vector<Spherical> sphericals;
};

#endif