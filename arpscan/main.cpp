#include "frmarpscan.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FrmArpScan w;
    w.show();

    return a.exec();
}
