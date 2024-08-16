#include "timeschedule.h"
#include <QtWidgets/QApplication>

#include <QDebug>

#include <QSet>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TimeSchedule w;
	w.setBarColor(Qt::green);
	w.setFontColor(Qt::black);
	w.setMainScaleMarkLenth(15);
    w.show();
    return a.exec();
}
