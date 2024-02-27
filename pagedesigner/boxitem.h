#pragma once

#include "itemtypes.hpp"
#include <QBrush>
#include <QGraphicsRectItem>
#include <QPen>

class QDataStream;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QKeyEvent;

class BoxItem  : public QObject, public QGraphicsRectItem
{
	Q_OBJECT
	Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
	Q_PROPERTY(QPen pen READ pen WRITE setPen)
	Q_PROPERTY(double angle READ angle WRITE setAngle)
	Q_PROPERTY(double shearHorizontal READ shearHorizontal
		WRITE setShearHorizontal)
	Q_PROPERTY(double shearVertical READ shearVertical
		WRITE setShearVertical)
public:
	enum { Type = BoxItemType };

	BoxItem(const QRect& rect, QGraphicsScene* scene);
	~BoxItem();

	int type() const { return Type; }

	double angle() const { return m_angle; }
	double shearHorizontal() const { return m_shearHorizontal; }
	double shearVertical() const { return m_shearVertical; }

signals:
	void dirty();

public slots:
	void setPen(const QPen& pen);
	void setBrush(const QBrush& brush);
	void setAngle(double angle);
	void setShearHorizontal(double shearHorizontal)
	{
		setShear(shearHorizontal, m_shearVertical);
	}
	void setShearVertical(double shearVertical)
	{
		setShear(m_shearHorizontal, shearVertical);
	}
	void setShear(double shearHorizontal, double shearVertical);

protected:
	QVariant itemChange(GraphicsItemChange change,
		const QVariant& value);
	void keyPressEvent(QKeyEvent* event);
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
	void updateTransform();

	bool m_resizing;
	double m_angle;
	double m_shearHorizontal;
	double m_shearVertical;
};

QDataStream& operator<<(QDataStream& out, const BoxItem& boxItem);
QDataStream& operator>>(QDataStream& in, BoxItem& boxItem);
