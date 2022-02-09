#ifndef __LOADER_H__
#define __LOADER_H__

#include <bits/stdc++.h>
#include "triangle.h"
#include "material.h"

class Loader
{
    std::vector<Triangle> triangles;
    std::map<std::string, Material*> material_dict;

public:
    ~Loader();
    void loadMtl(const std::string &filename);
    void loadObj(const std::string &filename, const vec3 &position, float scale, const Material* forcing_mat = nullptr);
    std::vector<Triangle>& getTriangles();
};


#endif
