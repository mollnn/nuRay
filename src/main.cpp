#include "gui/widget.h"

#include <QApplication>
#include "utils/utils.h"
#include "cmd/cmd.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (argc == 1)
    {
        Widget w;
        w.show();
        return a.exec();
    }
    else
    {
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
