#include "daemonproc.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DaemonProc w;
    w.show();
    return a.exec();
}
