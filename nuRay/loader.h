#ifndef __LOADER_H__
#define __LOADER_H__

#include <bits/stdc++.h>
#include "triangle.h"

class Loader
{
    std::vector<Triangle> triangles;

public:
    void loadObj(const std::string &filename, const vec3 &position, float scale);
    std::vector<Triangle>& getTriangles();
};


#endif
