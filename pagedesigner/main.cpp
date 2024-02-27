#include "pagedesigner.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	a.setApplicationName(a.translate("main", "Page Designer"));
	a.setOrganizationName("Qtrac Ltd.");
	a.setOrganizationDomain("qtrac.eu");
	a.setWindowIcon(QIcon(":/images/icon.png"));

    PageDesigner w;
    w.show();
    return a.exec();
}
