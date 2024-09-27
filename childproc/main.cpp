#include "childproc.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChildProc w;
    w.show();
    return a.exec();
}
