#include "image2image.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Image2Image w;
    w.show();
    return a.exec();
}
