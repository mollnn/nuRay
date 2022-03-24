#include "gui/widget.h"

#include <QApplication>
#include <QCoreApplication>
#include "utils/utils.h"
#include "cmd/cmd.h"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        QApplication a(argc, argv);
        Widget w;
        w.show();
        return a.exec();
    }
    else
    {
        QCoreApplication a(argc, argv);
        std::vector<std::string> args;
        for(int i=1;i<argc;i++)
        {
            args.push_back(std::string(argv[i]));
        }
        Cmd cmd;
        cmd.main(args);
        return 0;
    }
}
