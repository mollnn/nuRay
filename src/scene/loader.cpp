#include "../scene/loader.h"
#include "../material/matlight.h"
#include "../material/matlambert.h"
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

void Loader::fromConfig(Config &config)
{
    Material *material = new MatLight(1.0f);
    auto obj_list = config.getValueList("objects");
    for (auto obj_name : obj_list)
    {
        auto obj_type = config.getValueStr(obj_name + ".type", "");
        vec3 position = config.getValueVec3XYZ(obj_name + ".position", vec3());
        float scale = config.getValueFloat(obj_name + ".scale", 1.0f);
        if (obj_type == "from_obj_file")
        {
            auto obj_filename = config.getValueStr(obj_name + ".filename", "");
            loadObj(obj_filename, position, scale, material);
        }
    }
}

void Loader::fromSceneDescription(const std::string &scene_desc)
{
    // std::stringstream scene_desc_ss(scene_desc);
    // std::string desc_line;
    // primitives_.clear();
    // for (auto &[i, j] : material_dict)
    // {
    //     delete j;
    // }
    // material_dict.clear();
    // Material *forcing_mat = nullptr;
    // while (getline(scene_desc_ss, desc_line))
    // {
    //     std::stringstream desc_line_ss(desc_line);
    //     std::string filename;
    //     vec3 position = 0.0f;
    //     float scale = 1.0f;
    //     bool light = false;
    //     bool ggx = false;
    //     bool ggxr = false;
    //     bool glass = false;
    //     bool mirror = false;
    //     vec3 t_vec;
    //     float t_ps;
    //     float t_ps2;
    //     if (desc_line_ss >> filename)
    //     {
    //         std::string op;
    //         while (desc_line_ss >> op)
    //         {
    //             if (op == "-p")
    //             {
    //                 desc_line_ss >> position[0] >> position[1] >> position[2];
    //             }
    //             else if (op == "-s")
    //             {
    //                 desc_line_ss >> scale;
    //             }
    //             else if (op == "-m=light")
    //             {
    //                 desc_line_ss >> t_vec[0] >> t_vec[1] >> t_vec[2];
    //                 light = true;
    //             }
    //         }
    //         if (light)
    //         {
    //             forcing_mat = new MatLight(t_vec);
    //             material_dict["__light_" + std::to_string(rand() * rand()) + std::to_string(rand() * rand())] = forcing_mat;
    //         }
    //         loadObj(filename, position, scale, forcing_mat);
    //     }
    //     forcing_mat = nullptr;
    // }
}

void Loader::loadMtl(const std::string &filename)
{
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

            for (int i = 0; i < buf.size() - 1; i++)
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
                primitives_.push_back(Triangle(vertices[a[0][0]], vertices[a[1][0]], vertices[a[2][0]],
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
                primitives_.push_back(Triangle(vertices[a[0][0]], vertices[a[1][0]], vertices[a[2][0]],
                                               texcoords.size() > a[0][1] ? texcoords[a[0][1]] : vec3(0.0f, 0.0f, 0.0f),
                                               texcoords.size() > a[1][1] ? texcoords[a[1][1]] : vec3(0.0f, 0.0f, 0.0f),
                                               texcoords.size() > a[2][1] ? texcoords[a[2][1]] : vec3(0.0f, 0.0f, 0.0f),
                                               normals.size() > a[0][2] ? normals[a[0][2]] : vec3(0.0f, 0.0f, 0.0f),
                                               normals.size() > a[1][2] ? normals[a[1][2]] : vec3(0.0f, 0.0f, 0.0f),
                                               normals.size() > a[2][2] ? normals[a[2][2]] : vec3(0.0f, 0.0f, 0.0f),
                                               mtl));
                primitives_.push_back(Triangle(vertices[a[2][0]], vertices[a[3][0]], vertices[a[0][0]],
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
    return primitives_;
}

std::vector<float> Loader::getVerticesNormals()
{
    std::vector<float> ans;
    for (auto i : primitives_)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
                ans.push_back(i.p[j][k]);
            for (int k = 0; k < 3; k++)
                ans.push_back(i.n[j][k]);
        }
    }
    return ans;
}