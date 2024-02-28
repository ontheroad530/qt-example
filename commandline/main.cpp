#include "commandline.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CommandLine w;
    w.show();
    return a.exec();
}
