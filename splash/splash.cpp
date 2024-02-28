#include "splash.h"

#include <QThread>

Splash::Splash(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SplashClass())
{
    ui->setupUi(this);

	QThread::msleep(1000);
}

Splash::~Splash()
{
    delete ui;
}
