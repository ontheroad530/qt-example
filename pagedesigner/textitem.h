#pragma once

#include "itemtypes.hpp"
#include <QGraphicsScene>
#include <QGraphicsTextItem>

class TextItem  : public QGraphicsTextItem
{
	Q_OBJECT
	Q_PROPERTY(double angle READ angle WRITE setAngle)
	Q_PROPERTY(double shearHorizontal READ shearHorizontal
		WRITE setShearHorizontal)
	Q_PROPERTY(double shearVertical READ shearVertical
		WRITE setShearVertical)
public:
	enum { Type = TextItemType };
	TextItem(const QPoint& position, QGraphicsScene* scene);
	~TextItem();

	int type() const { return Type; }

	double angle() const { return m_angle; }
	double shearHorizontal() const { return m_shearHorizontal; }
	double shearVertical() const { return m_shearVertical; }

public slots:
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
	void edit();

signals:
	void dirty();

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant& value);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) { edit(); }

private:
	void updateTransform();

	double m_angle;
	double m_shearHorizontal;
	double m_shearVertical;
};

QDataStream& operator<<(QDataStream& out, const TextItem& textItem);
QDataStream& operator>>(QDataStream& in, TextItem& textItem);