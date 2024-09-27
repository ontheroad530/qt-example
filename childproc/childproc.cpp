#include "childproc.h"

#include <QTimer>

ChildProc::ChildProc(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChildProcClass())
{
    ui->setupUi(this);

	initForm();
}

ChildProc::~ChildProc()
{
    delete ui;
}

void ChildProc::initForm()
{
	m_localSocketName = "LOCAL_SOCKET_NAME";
	m_localSocket = QSharedPointer<QLocalSocket>(new QLocalSocket(this));

	connect(m_localSocket.get(), &QLocalSocket::disconnected, this, [this]() {
		QTimer::singleShot(500, [this]() {
			this->close();
		});
	});

	connect(m_localSocket.get(), &QLocalSocket::errorOccurred, this, [this](QLocalSocket::LocalSocketError err) {
		QTimer::singleShot(500, [this]() {
			this->close();
		});
	});

	connect(m_localSocket.get(), &QLocalSocket::readyRead, this, [this]() {
		QByteArray ba = m_localSocket->readAll();
		if ("CRASH_TEST_CMD" == ba)
		{
			qWarning("------test crash------------");
			//ÄÚ´æ±ÀÀ£
			int i = 0;
			int j = j / i;
			qDebug() << j;
		}
	});

	m_localSocket->connectToServer(m_localSocketName);
}
