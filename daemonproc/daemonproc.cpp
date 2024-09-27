#include "daemonproc.h"

#include <QLocalSocket>
#include <QLocalServer>

DaemonProc::DaemonProc(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DaemonProcClass())
	, m_proc(QSharedPointer<QProcess>(new QProcess()))
	, m_localServer(new QLocalServer(this))
{
    ui->setupUi(this);

	initForm();
}

DaemonProc::~DaemonProc()
{
    delete ui;
}

void DaemonProc::initForm()
{
	m_localSocketName = "LOCAL_SOCKET_NAME";

	//子进程退出监控
	connect(m_proc.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &DaemonProc::onProcFinished, Qt::UniqueConnection);

	//开启QLocalServer服务
	m_localServer->listen(m_localSocketName);
}

void DaemonProc::on_btnStartChild_clicked()
{
	startChildProc();
}

void DaemonProc::on_btnCrashChild_clicked()
{
	QLocalSocket* localSocket = m_localServer->nextPendingConnection();
	if (localSocket)
	{
		localSocket->write("CRASH_TEST_CMD");
		localSocket->flush();
	}
}

void DaemonProc::onProcFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (QProcess::CrashExit == exitStatus)
	{
		qWarning("CrashExit to restart");
		startChildProc();
	}
	else
	{
		qInfo("NormalExit to exit");
		qApp->exit();
	}
}

void DaemonProc::startChildProc()
{
	if (m_proc)
	{
		m_proc->start("childproc.exe", QStringList());
	}
}
