#include "splash.h"
#include <QtWidgets/QApplication>
#include <QSplashScreen>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if 1
	QPixmap pixmap(":/images/splash_logo.png");
	QSplashScreen splash(pixmap);
	splash.show();
	a.processEvents();
	
    Splash w;
    w.show();
	splash.finish(&w);
#else

	QPixmap pixmap(":/images/splash_logo.png");
	QSplashScreen* splash = new QSplashScreen(pixmap);
	splash->show();


	QThread::msleep(1000);
	splash->showMessage("Loaded modules", 1, "red");

	QCoreApplication::processEvents();

	QThread::msleep(2000);
	splash->showMessage("Established connections", 1, "white");

	QCoreApplication::processEvents();

#endif

    return a.exec();
}
