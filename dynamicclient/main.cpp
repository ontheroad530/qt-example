#include <QCoreApplication>

#include "dynamicclient.h"

#define ENABLE_REMOTE

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	QSharedPointer<QRemoteObjectDynamicReplica> ptr; // shared pointer to hold replica

#ifdef ENABLE_REMOTE
	QRemoteObjectNode repNode(QUrl(QStringLiteral("local:registry")));
#else
	QRemoteObjectNode repNode;
	repNode.connectToNode(QUrl(QStringLiteral("local:switch")));
#endif // ENABLE_REMOTE

	ptr.reset(repNode.acquireDynamic("SimpleSwitch")); // acquire replica of source from host node

	DynamicClient rswitch(ptr); // create client switch object and pass replica reference to it

	return a.exec();
}
