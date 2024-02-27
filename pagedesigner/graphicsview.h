#pragma once

#include <QtMath>
#include <QWheelEvent>
#include <QGraphicsView>

class GraphicsView  : public QGraphicsView
{
	Q_OBJECT

public:
	GraphicsView(QWidget* parent = 0)
		: QGraphicsView(parent)
	{
		setDragMode(RubberBandDrag);
		setRenderHints(QPainter::Antialiasing |
			QPainter::TextAntialiasing);
	}

	~GraphicsView() {}

public slots:
	void zoomIn() { scaleBy(1.1); }
	void zoomOut() { scaleBy(1.0 / 1.1); }

protected:
	void wheelEvent(QWheelEvent* event)
	{
		scaleBy(pow(4.0 / 3.0, (-event->delta() / 240.0)));
	}

private:
	void scaleBy(double factor) { scale(factor, factor); }
};
