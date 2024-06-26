#include "simpleswitch.h"

#include <QDebug>
#include <QTimer>

// constructor
SimpleSwitch::SimpleSwitch(QObject* parent) : SimpleSwitchSimpleSource(parent)
{
	stateChangeTimer = new QTimer(this); // Initialize timer
	QObject::connect(stateChangeTimer, SIGNAL(timeout()), this, SLOT(timeout_slot())); // connect timeout() signal from stateChangeTimer to timeout_slot() of simpleSwitch
	stateChangeTimer->start(2000); // Start timer and set timout to 2 seconds
	qDebug() << "Source Node Started";
}

//destructor
SimpleSwitch::~SimpleSwitch()
{
	stateChangeTimer->stop();
}

void SimpleSwitch::server_slot(bool clientState)
{
	qDebug() << "Replica state is " << clientState; // print switch state echoed back by client
}

void SimpleSwitch::timeout_slot()
{
	// slot called on timer timeout
	if (currState()) // check if current state is true, currState() is defined in repc generated rep_SimpleSwitch_source.h
		setCurrState(false); // set state to false
	else
		setCurrState(true); // set state to true
	qDebug() << "Source State is " << currState();

}
