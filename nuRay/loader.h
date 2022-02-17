#ifndef __LOADER_H__
#define __LOADER_H__

#include <bits/stdc++.h>
#include "triangle.h"
#include "material.h"

class Loader
{
    std::vector<Triangle> primitives_;
    std::map<std::string, Material*> material_dict;

public:
    ~Loader();
    void fromSceneDescription(const std::string &scene_desc);
    void loadMtl(const std::string &filename);
    void loadObj(const std::string &filename, const vec3 &position, float scale, const Material* forcing_mat = nullptr);
    std::vector<Triangle>& getTriangles();
    std::vector<float> getVerticesNormals();
};


#endif
