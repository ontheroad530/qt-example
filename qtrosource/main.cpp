#include <QtCore/QCoreApplication>
#include "simpleswitch.h"

#define ENABLE_REMOTE

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	SimpleSwitch srcSwitch; // create simple switch

#ifdef ENABLE_REMOTE
	QRemoteObjectRegistryHost regNode(QUrl(QStringLiteral("local:registry"))); // create node that hosts registry
	QRemoteObjectHost srcNode(QUrl(QStringLiteral("local:switch")), QUrl(QStringLiteral("local:registry"))); // create host node without Registry
#else
	QRemoteObjectHost srcNode(QUrl(QStringLiteral("local:switch"))); // create host node without Registry
#endif
	
	srcNode.enableRemoting(&srcSwitch); // enable remoting/sharing

    return a.exec();
}
