#ifndef _PATH_TRACER_HPP_
#define _PATH_TRACER_HPP_

#include <bits/stdc++.h>
using namespace std;

#include "vec3.hpp"
#include "timer.hpp"
#include "random.hpp"
#include "material.hpp"
#include "triangle.hpp"
#include "scene.hpp"

class PathTracer
{
public:
    vec3 PathTrace(vec3 raypos, vec3 raydir, int depth, const std::vector<Triangle> &triangles);
};

#endif