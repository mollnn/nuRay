#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <bits/stdc++.h>
#include "camera.h"
#include "triangle.h"
#include <QImage>

class Renderer 
{
    std::tuple<float, float, float, const Triangle*> intersect(const vec3& origin, const vec3& dir, const std::vector<Triangle>& triangles);
    vec3 trace(const vec3& orig, const vec3& dir,  const std::vector<Triangle>& triangles);
public:
    void render(const Camera& camera, const std::vector<Triangle>& triangles, QImage& img);
};

#endif