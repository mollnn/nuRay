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
        else if (buf[0] == "f")
        {
            // TODO: read normals and texcoords from obj
            std::vector<std::vector<int>> a(3);
            for (int i = 0; i < 3; i++)
            {
                std::vector<std::size_t> pos_slash;
                for (int j = 0; j < buf[i + 1].length(); j++)
                {
                    if (buf[i + 1][j] == '/')
                        pos_slash.push_back(j);
                }
                while (pos_slash.size() < 3)
                {
                    pos_slash.push_back(buf[i + 1].length());
                }
                std::size_t ptr = 0;
                for (int j = 0; j < 3; j++)
                {
                    std::size_t cur = pos_slash[j];
                    // ptr..cur-1
                    // nextptr = cur+1
                    if (ptr < cur)
                        a[i].push_back(atoi(buf[i + 1].substr(ptr, cur - ptr).c_str()));
                    else
                        a[i].push_back(0);
                    ptr = std::min(buf[i + 1].length(), cur + 1);
                }
            }
            std::cout<<a[0][0]<<" "<<a[0][1]<<" "<<a[0][2]<<"   "<<a[1][0]<<" "<<a[1][1]<<" "<<a[1][2]<<"   "<<a[2][0]<<" "<<a[2][1]<<" "<<a[2][2]<<std::endl;

            triangles.push_back(Triangle(vertices[a[0][0]],vertices[a[1][0]],vertices[a[2][0]],
                texcoords.size() > a[0][1] ? texcoords[a[0][1]] : vec3(0.0f, 0.0f, 0.0f),
                texcoords.size() > a[1][1] ? texcoords[a[1][1]] : vec3(0.0f, 0.0f, 0.0f),
                texcoords.size() > a[2][1] ? texcoords[a[2][1]] : vec3(0.0f, 0.0f, 0.0f)
            ));
        }
    }
}

std::vector<Triangle> &Loader::getTriangles()
{
    return triangles;
}