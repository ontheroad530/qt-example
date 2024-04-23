#pragma once

#include "rep_SimpleSwitch_source.h"

class SimpleSwitch  : public SimpleSwitchSimpleSource
{
	Q_OBJECT

public:
	SimpleSwitch(QObject *parent = nullptr);
	~SimpleSwitch();

	virtual void server_slot(bool clientState);
public Q_SLOTS:
	void timeout_slot();
private:
	QTimer* stateChangeTimer;
};
