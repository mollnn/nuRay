#include "loader.h"
#include "matlight.h"
#include "matlambert.h"
#include "matblinnphong.h"
#include <QDebug>

Loader::~Loader()
{
    for (auto &[i, j] : material_dict)
    {
        delete j;
    }
}

std::string path2dir(const std::string &path)
{
    std::size_t pos = path.find_last_of("/\\");
    if (pos == path.npos)
        return ".";
    return path.substr(0, pos);
}

void Loader::loadMtl(const std::string &filename)
{
    float Ns, Ni;
    vec3 Ka, Kd, Ks, Ke, Tf;
    std::string map_Kd, map_Ks;
    std::string name;

    auto commit = [&]()
    {
        Material *mat;
        if (Ke.norm2() > 1e-6)
        {
            mat = new MatLight(Ke);
        }
        else
        {
            if (Ks.norm2() > 1e-6 || map_Ks != "")
            {
                MatBlinnPhong *bf = new MatBlinnPhong(Kd, Ks, Ns);
                if (map_Kd != "")
                {
                    bf->usetex_Kd_ = true;
                    bf->map_Kd_.load(map_Kd);
                }
                if (map_Ks != "")
                {
                    bf->usetex_Ks_ = true;
                    bf->map_Ks_.load(map_Ks);
                }
                mat = bf;
            }
            else
            {
                MatLambert *lambert = new MatLambert(Kd);
                if (map_Kd != "")
                {
                    lambert->usetex_Kd_ = true;
                    lambert->map_Kd_.load(map_Kd);
                }
                mat = lambert;
            }
        }
        material_dict[filename + ":" + name] = mat;

        Ns = 10.0f;
        Ni = 1.0f;
        Ka = 0.0f;
        Kd = 0.0f;
        Ks = 0.0f;
        Ke = 0.0f;
        Tf = 0.0f;
        map_Kd = "";
        map_Ks = "";

        return;
    };

    std::ifstream ifs(filename);
    std::string buf_line;

    while (std::getline(ifs, buf_line))
    {
        if (buf_line.size() > 0 && buf_line[0] == '#')
            continue;
        std::stringstream ss(buf_line);
        std::vector<std::string> buf;
        std::string tmp;
        while (ss >> tmp)
        {
            buf.push_back(tmp);
        }
        if (buf.size() == 0)
            continue;
        // ...
        if (buf[0] == "newmtl")
        {
            // Commit material
            if (name != "")
                commit();
            name = buf[1];
        }
        else if (buf[0] == "Ns")
        {
            Ns = atof(buf[1].c_str());
        }
        else if (buf[0] == "Ni")
        {
            Ni = atof(buf[1].c_str());
        }
        else if (buf[0] == "Ka")
        {
            Ka = vec3(atof(buf[1].c_str()), atof(buf[2].c_str()), atof(buf[3].c_str()));
        }
        else if (buf[0] == "Kd")
        {
            Kd = vec3(atof(buf[1].c_str()), atof(buf[2].c_str()), atof(buf[3].c_str()));
        }
        else if (buf[0] == "map_Kd")
        {
            map_Kd = path2dir(filename) + "/" + buf[1];
        }
        else if (buf[0] == "map_Ks")
        {
            map_Ks = path2dir(filename) + "/" + buf[1];
        }
        else if (buf[0] == "Ks")
        {
            Ks = vec3(atof(buf[1].c_str()), atof(buf[2].c_str()), atof(buf[3].c_str()));
        }
        else if (buf[0] == "Ke")
        {
            Ke = vec3(atof(buf[1].c_str()), atof(buf[2].c_str()), atof(buf[3].c_str()));
        }
        else if (buf[0] == "Tf")
        {
            Tf = vec3(atof(buf[1].c_str()), atof(buf[2].c_str()), atof(buf[3].c_str()));
        }
    }
    if (name != "")
        commit();
}

void Loader::loadObj(const std::string &filename, const vec3 &position, float scale, const Material *forcing_mat)
{
    std::vector<vec3> vertices(1), normals(1), texcoords(1);
    std::ifstream ifs(filename);
    std::string buf_line;
    std::string mtllib_filename;
    const Material *mtl = forcing_mat;
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
        if (buf[0] == "mtllib")
        {
            mtllib_filename = path2dir(filename) + "/" + buf[1];
            if (forcing_mat == nullptr)
            {
                loadMtl(mtllib_filename);
            }
        }
        else if (buf[0] == "usemtl")
        {
            if (forcing_mat == nullptr)
            {
                std::string mtl_name = buf[1];
                std::string mtl_fullname = mtllib_filename + ":" + mtl_name;
                if (material_dict.find(mtl_fullname) == material_dict.end())
                {
                    std::cerr << "Cannot find material " << mtl_fullname << std::endl;
                }
                mtl = material_dict[mtl_fullname];
            }
        }
        else if (buf[0] == "v")
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
            std::vector<std::vector<int>> a(buf.size() - 1);
            for (int i = 0; i < buf.size() - 1; i++)
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

            // std::cout << (texcoords.size() > a[0][1] ? texcoords[a[0][1]] : vec3(0.0f, 0.0f, 0.0f)) << (texcoords.size() > a[1][1] ? texcoords[a[1][1]] : vec3(0.0f, 0.0f, 0.0f)) << (texcoords.size() > a[2][1] ? texcoords[a[2][1]] : vec3(0.0f, 0.0f, 0.0f)) << std::endl;

            for (int i = 0; i < 3; i++)
            {
                if (a[i][0] < 0)
                    a[i][0] += vertices.size();
                if (a[i][1] < 0)
                    a[i][1] += texcoords.size();
                if (a[i][2] < 0)
                    a[i][2] += normals.size();
            }

            if (buf.size() - 1 == 3)
            {
                triangles.push_back(Triangle(vertices[a[0][0]], vertices[a[1][0]], vertices[a[2][0]],
                                             texcoords.size() > a[0][1] ? texcoords[a[0][1]] : vec3(0.0f, 0.0f, 0.0f),
                                             texcoords.size() > a[1][1] ? texcoords[a[1][1]] : vec3(0.0f, 0.0f, 0.0f),
                                             texcoords.size() > a[2][1] ? texcoords[a[2][1]] : vec3(0.0f, 0.0f, 0.0f),
                                             normals.size() > a[0][2] ? normals[a[0][2]] : vec3(0.0f, 0.0f, 0.0f),
                                             normals.size() > a[1][2] ? normals[a[1][2]] : vec3(0.0f, 0.0f, 0.0f),
                                             normals.size() > a[2][2] ? normals[a[2][2]] : vec3(0.0f, 0.0f, 0.0f),
                                             mtl));
            }
            else if (buf.size() - 1 == 4)
            {
                triangles.push_back(Triangle(vertices[a[0][0]], vertices[a[1][0]], vertices[a[2][0]],
                                             texcoords.size() > a[0][1] ? texcoords[a[0][1]] : vec3(0.0f, 0.0f, 0.0f),
                                             texcoords.size() > a[1][1] ? texcoords[a[1][1]] : vec3(0.0f, 0.0f, 0.0f),
                                             texcoords.size() > a[2][1] ? texcoords[a[2][1]] : vec3(0.0f, 0.0f, 0.0f),
                                             normals.size() > a[0][2] ? normals[a[0][2]] : vec3(0.0f, 0.0f, 0.0f),
                                             normals.size() > a[1][2] ? normals[a[1][2]] : vec3(0.0f, 0.0f, 0.0f),
                                             normals.size() > a[2][2] ? normals[a[2][2]] : vec3(0.0f, 0.0f, 0.0f),
                                             mtl));
                triangles.push_back(Triangle(vertices[a[2][0]], vertices[a[3][0]], vertices[a[0][0]],
                                             texcoords.size() > a[2][1] ? texcoords[a[2][1]] : vec3(0.0f, 0.0f, 0.0f),
                                             texcoords.size() > a[3][1] ? texcoords[a[3][1]] : vec3(0.0f, 0.0f, 0.0f),
                                             texcoords.size() > a[0][1] ? texcoords[a[0][1]] : vec3(0.0f, 0.0f, 0.0f),
                                             normals.size() > a[2][2] ? normals[a[2][2]] : vec3(0.0f, 0.0f, 0.0f),
                                             normals.size() > a[3][2] ? normals[a[3][2]] : vec3(0.0f, 0.0f, 0.0f),
                                             normals.size() > a[0][2] ? normals[a[0][2]] : vec3(0.0f, 0.0f, 0.0f),
                                             mtl));
            }
            else
            {
                std::cerr << "Unsupported f type" << std::endl;
            }
        }
    }
}

std::vector<Triangle> &Loader::getTriangles()
{
    return triangles;
}