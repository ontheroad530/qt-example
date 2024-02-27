#include "penwidget.h"
#include "../aqp/alt_key.hpp"
#include "swatch.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QSpinBox>

PenWidget::PenWidget(QWidget *parent)
	: QWidget(parent)
{
	createWidgets();
	setPen(QPen());
	createLayout();
	createConnections();
	setFixedSize(minimumSizeHint());
}

PenWidget::~PenWidget()
{}

void PenWidget::setPen(const QPen& pen)
{
	if (m_pen != pen) {
		m_pen = pen;
		colorComboBox->setCurrentIndex(
			colorComboBox->findData(m_pen.color()));
		widthSpinBox->setValue(m_pen.width());
		lineStyleComboBox->setCurrentIndex(
			lineStyleComboBox->findData(
				static_cast<int>(m_pen.style())));
		capStyleComboBox->setCurrentIndex(
			capStyleComboBox->findData(
				static_cast<int>(m_pen.capStyle())));
		joinStyleComboBox->setCurrentIndex(
			joinStyleComboBox->findData(
				static_cast<int>(m_pen.joinStyle())));
	}
}

void PenWidget::updateColor(int index)
{
	m_pen.setColor(colorComboBox->itemData(index).value<QColor>());
	updateSwatches();
	emit penChanged(m_pen);
}

void PenWidget::updateWidth(int value)
{
	m_pen.setWidth(value);
	emit penChanged(m_pen);
}

void PenWidget::updateLineStyle(int index)
{
	m_pen.setStyle(static_cast<Qt::PenStyle>(
		lineStyleComboBox->itemData(index).toInt()));
	emit penChanged(m_pen);
}

void PenWidget::updateCapStyle(int index)
{
	m_pen.setCapStyle(static_cast<Qt::PenCapStyle>(
		capStyleComboBox->itemData(index).toInt()));
	emit penChanged(m_pen);
}

void PenWidget::updateJoinStyle(int index)
{
	m_pen.setJoinStyle(static_cast<Qt::PenJoinStyle>(
		joinStyleComboBox->itemData(index).toInt()));
	emit penChanged(m_pen);
}

void PenWidget::createWidgets()
{
	colorComboBox = new QComboBox;
	foreach(const QString & name, QColor::colorNames()) {
		QColor color(name);
		colorComboBox->addItem(colorSwatch(color), name, color);
	}

	widthSpinBox = new QSpinBox;
	widthSpinBox->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	widthSpinBox->setRange(0, 20);
	widthSpinBox->setSuffix(tr(" px"));
	widthSpinBox->setSpecialValueText(tr("1 px (fine)"));

	lineStyleComboBox = new QComboBox;
	typedef QPair<QString, Qt::PenStyle> PenPair;
	foreach(const PenPair& pair, QList<PenPair>()
		<< qMakePair(tr("No Pen"), Qt::NoPen)
		<< qMakePair(tr("Solid"), Qt::SolidLine)
		<< qMakePair(tr("Dashed"), Qt::DashLine)
		<< qMakePair(tr("Dotted"), Qt::DotLine)
		<< qMakePair(tr("Dash-Dotted"), Qt::DashDotLine)
		<< qMakePair(tr("Dash-Dot-Dotted"), Qt::DashDotDotLine))
		lineStyleComboBox->addItem(penStyleSwatch(pair.second),
			pair.first, (int)pair.second);

	capStyleComboBox = new QComboBox;
	capStyleComboBox->addItem(penCapSwatch(Qt::FlatCap),
		tr("Flat"), Qt::FlatCap);
	capStyleComboBox->addItem(penCapSwatch(Qt::RoundCap),
		tr("Round"), Qt::RoundCap);
	capStyleComboBox->addItem(penCapSwatch(Qt::SquareCap),
		tr("Square"), Qt::SquareCap);

	joinStyleComboBox = new QComboBox;
	joinStyleComboBox->addItem(penJoinSwatch(Qt::BevelJoin),
		tr("Join"), Qt::BevelJoin);
	joinStyleComboBox->addItem(penJoinSwatch(Qt::MiterJoin),
		tr("Miter"), Qt::MiterJoin);
	joinStyleComboBox->addItem(penJoinSwatch(Qt::RoundJoin),
		tr("Round"), Qt::RoundJoin);
}

void PenWidget::createLayout()
{
	QFormLayout* layout = new QFormLayout;
	layout->addRow(tr("Color"), colorComboBox);
	layout->addRow(tr("Width"), widthSpinBox);
	layout->addRow(tr("Style"), lineStyleComboBox);
	layout->addRow(tr("Cap"), capStyleComboBox);
	layout->addRow(tr("Join"), joinStyleComboBox);
	setLayout(layout);
}

void PenWidget::createConnections()
{
	connect(colorComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateColor(int)));
	connect(widthSpinBox, SIGNAL(valueChanged(int)),
		this, SLOT(updateWidth(int)));
	connect(lineStyleComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateLineStyle(int)));
	connect(capStyleComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateCapStyle(int)));
	connect(joinStyleComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateJoinStyle(int)));
}

void PenWidget::updateSwatches()
{
	QColor color = colorComboBox->itemData(
		colorComboBox->currentIndex()).value<QColor>();
	for (int i = 0; i < lineStyleComboBox->count(); ++i)
		lineStyleComboBox->setItemIcon(i, penStyleSwatch(
			static_cast<Qt::PenStyle>(
				lineStyleComboBox->itemData(i).toInt()), color));
	for (int i = 0; i < capStyleComboBox->count(); ++i)
		capStyleComboBox->setItemIcon(i, penCapSwatch(
			static_cast<Qt::PenCapStyle>(
				capStyleComboBox->itemData(i).toInt()), color));
	for (int i = 0; i < joinStyleComboBox->count(); ++i)
		joinStyleComboBox->setItemIcon(i, penJoinSwatch(
			static_cast<Qt::PenJoinStyle>(
				joinStyleComboBox->itemData(i).toInt()), color));
}
