#pragma once

#include <QProcess>
#include <QSharedPointer>
#include <QtWidgets/QWidget>

#include "ui_daemonproc.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DaemonProcClass; };
QT_END_NAMESPACE

class QLocalServer;
class DaemonProc : public QWidget
{
    Q_OBJECT

public:
    DaemonProc(QWidget *parent = nullptr);
    ~DaemonProc();

private Q_SLOTS:
	void initForm();

	void on_btnStartChild_clicked();
	void on_btnCrashChild_clicked();

	void onProcFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	void startChildProc();

private:
    Ui::DaemonProcClass *ui;

	QSharedPointer<QProcess>		m_proc;
	QSharedPointer<QLocalServer>	m_localServer;
	QString							m_localSocketName;
};
