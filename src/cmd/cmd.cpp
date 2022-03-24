#include "cmd.h"

void Cmd::main(const std::vector<std::string> &args)
{
    Config config;

    // Cmds are seped by spaces
    // Format: <scene desc> --output <output filename> --params <render setting>
    // Example: ./nuRay.exe  ../scenes/cornell/CornellBox-Mirror.obj -s 100  --output result.bmp  --params renderer=ptnee imgw=320 imgh=240 spp=16 campos=0,120,200 cameuler=0,0,0 camfov=64 camasp=1.333
    // DO NOT add any space in one kvpair

    std::string scene_desc;
    std::string output_filename;

    int stage = 0;
    for (auto &s : args)
    {
        if (s == "--output")
        {
            stage = 1;
        }
        else if (s == "--params")
        {
            stage = 2;
        }
        if (stage == 0)
        {
            scene_desc += s;
            scene_desc += " ";
        }
        else if (stage == 1)
        {
            if (s != "--output")
            {
                output_filename = s;
            }
        }
        else if (stage == 2)
        {
            config.addItem(s);
        }
    }

    std::cout << "config list" << std::endl;
    config.print();

    Loader loader;
    loader.fromSceneDescription(scene_desc);

    QImage result;
    QMutex mutex;

    Texture tex_envmap;
    if (config.getValueStr("envmap", "") != "")
    {
        tex_envmap.load(config.getValueStr("envmap", ""));
    }

    Envmap envmap(&tex_envmap, config.getValueFloat("envmap_gain", 1.0f));

    std::atomic<int> con_flag = 1;

    std::string renderer_name = config.getValueStr("renderer", "ptnee");
    RendererPTLS r;
    r.prepare(loader.getTriangles());
    r.render(
        Camera(config.getValueFloat("camfov", 90.0f), config.getValueFloat("camasp", 1.0f),
               config.getValueVec3("campos", {0, 0, 0}), config.getValueVec3("cameuler", {0, 0, 0})[0],
               config.getValueVec3("cameuler", {0, 0, 0})[1], config.getValueVec3("cameuler", {0, 0, 0})[2]),
        loader.getTriangles(),
        result,
        config,
        [&](bool) {},
        con_flag,
        [&](float x)
        { std::cout << "Process: " + std::to_string(x) + "% \t"; },
        mutex,
        config.getValueStr("envmap", "") == "" ? nullptr : &envmap);

    result.save(QString::fromStdString(output_filename));
    std::cout << "finish:)" << std::endl;
}