#include "dynamicclient.h"

// constructor
DynamicClient::DynamicClient(QSharedPointer<QRemoteObjectDynamicReplica> ptr) :
	QObject(nullptr), reptr(ptr)
{

	//connect signal for replica valid changed with signal slot initialization
	QObject::connect(reptr.data(), SIGNAL(initialized()), this, SLOT(initConnection_slot()));
}

//destructor
DynamicClient::~DynamicClient()
{
}

// Function to initialize connections between slots and signals
void DynamicClient::initConnection_slot()
{

	// connect source replica signal currStateChanged() with client's recSwitchState() slot to receive source's current state
	QObject::connect(reptr.data(), SIGNAL(currStateChanged(bool)), this, SLOT(recSwitchState_slot()));
	// connect client's echoSwitchState(..) signal with replica's server_slot(..) to echo back received state
	QObject::connect(this, SIGNAL(echoSwitchState(bool)), reptr.data(), SLOT(server_slot(bool)));
}

void DynamicClient::recSwitchState_slot()
{
	clientSwitchState = reptr->property("currState").toBool(); // use replica property to get currState from source
	qDebug() << "Received source state " << clientSwitchState;
	Q_EMIT echoSwitchState(clientSwitchState); // Emit signal to echo received state back to server
}
