#pragma once

#include "itemtypes.hpp"
#include <QGraphicsObject>
#include <QPainterPath>
#include <QPen>

class QAction;
class QActionGroup;
class QGraphicsScene;

class SmileyItem  : public QGraphicsObject
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
	enum Face { Happy, Sad, Neutral };
	enum { Type = SmileyItemType };

	SmileyItem(const QPoint& position, QGraphicsScene* scene);
	~SmileyItem();

	int type() const { return Type; }

	double angle() const { return m_angle; }
	double shearHorizontal() const { return m_shearHorizontal; }
	double shearVertical() const { return m_shearVertical; }

	QPen pen() const { return m_pen; }
	QBrush brush() const { return m_brush; }
	Face face() const { return m_face; }
	bool isShowingHat() const { return m_showHat; }

	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option, QWidget* widget);
	QRectF boundingRect() const;
	QPainterPath shape() const;

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
	void setFace(Face face);
	void setShowHat(bool on);
	void edit();

protected:
	QVariant itemChange(GraphicsItemChange change,
		const QVariant& value);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) { edit(); }
	void contextMenuEvent(QGraphicsSceneContextMenuEvent*) { edit(); }

private:
	void updateTransform();
	void paintFace(QPainter* painter);
	void paintEyes(QPainter* painter, int leftX, int rightX);
	void paintMouth(QPainter* painter, int leftX, int rightX);
	void paintHat(QPainter* painter);
	void paintSelectionOutline(QPainter* painter);
	void createPaths();
	QAction* createMenuAction(QMenu* menu, const QIcon& icon,
		const QString& text, bool checked,
		QActionGroup* group = 0, const QVariant& data = QVariant());

	Face m_face;
	QPen m_pen;
	QBrush m_brush;
	bool m_showHat;
	QPainterPath m_facePath;
	QPainterPath m_hatPath;
	double m_angle;
	double m_shearHorizontal;
	double m_shearVertical;
};

QDataStream& operator<<(QDataStream& out,
	const SmileyItem& smileyItem);
QDataStream& operator>>(QDataStream& in, SmileyItem& smileyItem);
