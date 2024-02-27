#pragma once

#include <QPen>
#include <QWidget>

class QComboBox;
class QSpinBox;

class PenWidget  : public QWidget
{
	Q_OBJECT

public:
	PenWidget(QWidget *parent = 0);
	~PenWidget();

	QPen pen() const { return m_pen; }

signals:
	void penChanged(const QPen& pen);

public slots:
	void setPen(const QPen& pen);

private slots:
	void updateColor(int index);
	void updateWidth(int value);
	void updateLineStyle(int index);
	void updateCapStyle(int index);
	void updateJoinStyle(int index);

private:
	void createWidgets();
	void createLayout();
	void createConnections();
	void updateSwatches();

	QComboBox* colorComboBox;
	QSpinBox* widthSpinBox;
	QComboBox* lineStyleComboBox;
	QComboBox* capStyleComboBox;
	QComboBox* joinStyleComboBox;

	QPen m_pen;
};
