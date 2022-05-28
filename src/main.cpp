#include <QApplication>
#include <QCoreApplication>
#include "utils/utils.h"
#include "cmd/cmd.h"

int main(int argc, char *argv[])
{
    std::cout << "nuRay" << std::endl;
    std::cout << "cmdmode of nuRay" << std::endl;
    QCoreApplication a(argc, argv);
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
    {
        args.push_back(std::string(argv[i]));
    }
    Cmd cmd;
    cmd.main(args);
    return 0;
}
