#include "zipcodespinbox.h"
#include "global.h"

ZipcodeSpinBox::ZipcodeSpinBox(QWidget* parent)
	: QSpinBox(parent)
{
	setRange(MinZipcode, MaxZipcode);
	setAlignment(Qt::AlignVCenter | Qt::AlignRight);
}

ZipcodeSpinBox::~ZipcodeSpinBox()
{}
