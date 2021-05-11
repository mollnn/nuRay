#include "minipt.hpp"

Minipt::Minipt()
{
}

void Minipt::LoadDefaultLightset()
{
    // Load scene

    scene.triangles.push_back({{0, -100, 100}, {0, 100, 100}, {100, 100, 100}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0.9, 0.9, 1.5}, 1}});
    scene.triangles.push_back({{0, -100, 100}, {100, -100, 100}, {100, 100, 100}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1.6, 1.2, 0.9}, 1}});

    scene.triangles.push_back({{-100, -100, -3}, {-100, 100, -3}, {100, 100, -3}, {{0.5, 0.5, 0.5}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 1}});
    scene.triangles.push_back({{-100, -100, -3}, {100, -100, -3}, {100, 100, -3}, {{0.5, 0.5, 0.5}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 1}});
}

void Minipt::ImportObject(std::string obj_path)
{
    // Load .obj File
    bool loadout = Loader.LoadFile(obj_path);
    for (auto mesh : Loader.LoadedMeshes)
    {
        cout << "New face loaded" << endl;
        for (int i = 0; i < mesh.Vertices.size(); i += 3)
        {
            scene.triangles.push_back({{mesh.Vertices[i + 0].Position.X, mesh.Vertices[i + 0].Position.Y, mesh.Vertices[i + 0].Position.Z},
                                       {mesh.Vertices[i + 1].Position.X, mesh.Vertices[i + 1].Position.Y, mesh.Vertices[i + 1].Position.Z},
                                       {mesh.Vertices[i + 2].Position.X, mesh.Vertices[i + 2].Position.Y, mesh.Vertices[i + 2].Position.Z},
                                       {{1, 1, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 1}});
        }
    }
}

void Minipt::AutoNormal()
{
    // Auto normal

    for (auto &i : scene.triangles)
        i.auto_normal();
}

Image Minipt::Render()
{
    // Prepare rendering

    Image image(img_siz_x, img_siz_y);

    int img_width = image.size_x;
    int img_height = image.size_y;

    int screen_w = img_siz_x;
    int screen_h = img_siz_y;
    int image_w = img_siz_x;
    int image_h = img_siz_y;

    // Render

    RenderMain(img_siz_x, img_siz_y, spp, cam_pos, cam_dir, cam_top, focal, near_clip, image, scene);

    // Save output image

    return image;
}