#pragma once

#include <QObject>
#include <QSharedPointer>

#include <QRemoteObjectNode>
#include <qremoteobjectdynamicreplica.h>

class DynamicClient : public QObject
{
	Q_OBJECT
public:
	DynamicClient(QSharedPointer<QRemoteObjectDynamicReplica> ptr);
	~DynamicClient();

Q_SIGNALS:
	void echoSwitchState(bool switchState);// this signal is connected with server_slot(..) slot of source object and echoes back switch state received from source

public Q_SLOTS:
	void recSwitchState_slot(); // Slot to receive source state
	void initConnection_slot(); //Slot to connect signals/slot on replica initialization

private:
	bool clientSwitchState; // holds received server switch state
	QSharedPointer<QRemoteObjectDynamicReplica> reptr;// holds reference to replica
};
