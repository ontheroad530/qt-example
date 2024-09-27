#pragma once

#include <QLocalSocket>
#include <QSharedPointer>
#include <QtWidgets/QWidget>

#include "ui_childproc.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ChildProcClass; };
QT_END_NAMESPACE


class ChildProc : public QWidget
{
    Q_OBJECT

public:
    ChildProc(QWidget *parent = nullptr);
    ~ChildProc();

private:
	void initForm();

private:
    Ui::ChildProcClass *ui;

	QSharedPointer<QLocalSocket>	m_localSocket;
	QString							m_localSocketName;
};
