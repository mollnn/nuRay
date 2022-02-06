#include "loader.h"

void Loader::loadObj(const std::string &filename, const vec3 &position, float scale)
{
    std::vector<vec3> vertices(1), normals(1), texcoords(1);
    std::ifstream ifs(filename);
    std::string buf_line;
    while (std::getline(ifs, buf_line))
    {
        std::stringstream ss(buf_line);
        std::vector<std::string> buf;
        std::string tmp;
        while (ss >> tmp)
        {
            buf.push_back(tmp);
        }
        if (buf.size() == 0)
            continue;
        if (buf[0] == "v")
        {
            // std::cout << buf[1] <<" "<<buf[2]<<" "<<buf[3]<<" "<<std::endl;
            vertices.push_back(vec3(atof(buf[1].c_str()), atof(buf[2].c_str()), atof(buf[3].c_str())) * scale + position);
        }
        else if (buf[0] == "vt")
        {
            texcoords.push_back(vec3(atof(buf[1].c_str()), atof(buf[2].c_str()), 0.0f));
        }
        else if (buf[0] == "vn")
        {
            normals.push_back(vec3(atof(buf[1].c_str()), atof(buf[2].c_str()), atof(buf[3].c_str())));
        }
        else if(buf[0] == "f")
        {
            // TODO: read normals and texcoords from obj
            triangles.push_back(Triangle(vertices[atoi(buf[1].c_str())],vertices[atoi(buf[2].c_str())],vertices[atoi(buf[3].c_str())]));
        }
    }
}


std::vector<Triangle>& Loader::getTriangles()
{
    return triangles;
}