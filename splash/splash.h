#pragma once

#include <QtWidgets/QWidget>
#include "ui_splash.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SplashClass; };
QT_END_NAMESPACE

class Splash : public QWidget
{
    Q_OBJECT

public:
    Splash(QWidget *parent = nullptr);
    ~Splash();

private:
    Ui::SplashClass *ui;
};
