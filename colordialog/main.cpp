
#include <QColorDialog>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QColorDialog w;
    w.show();
    return a.exec();
}
