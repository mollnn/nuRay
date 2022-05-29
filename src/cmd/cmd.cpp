#include "cmd.h"
#include "../renderer/rendererpt.h"
#include <QImage>

void Cmd::main(const std::vector<std::string> &args)
{
    Config config;
    if (args.size() == 0)
    {
        std::cerr << "No input file!" << std::endl;
        exit(1);
    }
    if (args.size() == 1)
    {
        std::cerr << "No output file!" << std::endl;
        exit(1);
    }

    config.fromFile(args[0]);
    config.print();

    Loader loader;
    loader.fromConfig(config);

    std::string output_filename = args[1];

    QImage result;
    QMutex mutex;

    Texture tex_envmap;
    if (config.getValueStr("envmap", "") != "")
    {
        tex_envmap.load(config.getValueStr("envmap", ""));
    }

    Envmap envmap(&tex_envmap, config.getValueFloat("envmap_gain", 1.0f));

    std::atomic<int> con_flag = 1;

    std::string renderer_name = config.getValueStr("renderer", "pt");
    std::shared_ptr<Renderer> r;

    if (renderer_name == "pt")
    {
        r = std::make_shared<RendererPT>();
    }

    r->prepare(loader.getTriangles());
    r->render(
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