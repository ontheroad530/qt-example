#pragma once

#include <QSpinBox>

class ZipcodeSpinBox  : public QSpinBox
{
	Q_OBJECT

public:
	ZipcodeSpinBox(QWidget* parent = 0);
	~ZipcodeSpinBox();

protected:
	QString textFromValue(int value) const
	{
		return QString("%1").arg(value, 5, 10, QChar('0'));
	}
};
