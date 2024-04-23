#include <QtCore/QCoreApplication>

#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	QSharedPointer<SimpleSwitchReplica> ptr; // shared pointer to hold source replica

	QRemoteObjectNode repNode; // create remote object node
	repNode.connectToNode(QUrl(QStringLiteral("local:switch"))); // connect with remote host node

	ptr.reset(repNode.acquire<SimpleSwitchReplica>()); // acquire replica of source from host node

	Client rswitch(ptr); // create client switch object and pass reference of replica to it

	return a.exec();
}
