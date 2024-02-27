#include "boxitem.h"
#include "../aqp/aqp.hpp"
#include "../aqp/alt_key.hpp"
#include "global.hpp"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

BoxItem::BoxItem(const QRect& rect_, QGraphicsScene* scene)
	: QObject(), QGraphicsRectItem(), m_resizing(false),
	m_angle(0.0), m_shearHorizontal(0.0), m_shearVertical(0.0)
{
	setFlags(QGraphicsItem::ItemIsSelectable |
#if QT_VERSION >= 0x040600
		QGraphicsItem::ItemSendsGeometryChanges |
#endif
		QGraphicsItem::ItemIsMovable |
		QGraphicsItem::ItemIsFocusable);
	setPos(rect_.center());
	setRect(QRectF(QPointF(-rect_.width() / 2.0,
		-rect_.height() / 2.0), rect_.size()));
	scene->clearSelection();
	scene->addItem(this);
	setSelected(true);
	setFocus();
}

BoxItem::~BoxItem()
{}

void BoxItem::setPen(const QPen& pen_)
{
	if (isSelected() && pen_ != pen()) {
		QGraphicsRectItem::setPen(pen_);
		emit dirty();
	}
}

void BoxItem::setBrush(const QBrush& brush_)
{
	if (isSelected() && brush_ != brush()) {
		QGraphicsRectItem::setBrush(brush_);
		emit dirty();
	}
}

void BoxItem::setAngle(double angle)
{
	if (isSelected() && !qFuzzyCompare(m_angle, angle)) {
		m_angle = angle;
		updateTransform();
	}
}

void BoxItem::setShear(double shearHorizontal, double shearVertical)
{
	if (isSelected() &&
		(!qFuzzyCompare(m_shearHorizontal, shearHorizontal) ||
			!qFuzzyCompare(m_shearVertical, shearVertical))) {
		m_shearHorizontal = shearHorizontal;
		m_shearVertical = shearVertical;
		updateTransform();
	}
}

QVariant BoxItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (isDirtyChange(change))
		emit dirty();
	return QGraphicsRectItem::itemChange(change, value);
}

void BoxItem::keyPressEvent(QKeyEvent* event)
{
	if (event->modifiers() & Qt::ShiftModifier ||
		event->modifiers() & Qt::ControlModifier) {
		bool move = event->modifiers() & Qt::ControlModifier;
		bool changed = true;
		double dx1 = 0.0;
		double dy1 = 0.0;
		double dx2 = 0.0;
		double dy2 = 0.0;
		switch (event->key()) {
		case Qt::Key_Left:
			if (move)
				dx1 = -1.0;
			dx2 = -1.0;
			break;
		case Qt::Key_Right:
			if (move)
				dx1 = 1.0;
			dx2 = 1.0;
			break;
		case Qt::Key_Up:
			if (move)
				dy1 = -1.0;
			dy2 = -1.0;
			break;
		case Qt::Key_Down:
			if (move)
				dy1 = 1.0;
			dy2 = 1.0;
			break;
		default:
			changed = false;
		}
		if (changed) {
			setRect(rect().adjusted(dx1, dy1, dx2, dy2));
			event->accept();
			emit dirty();
			return;
		}
	}
	QGraphicsRectItem::keyPressEvent(event);
}

void BoxItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->modifiers() & Qt::ShiftModifier) {
		m_resizing = true;
		setCursor(Qt::SizeAllCursor);
	}
	else
		QGraphicsRectItem::mousePressEvent(event);
}

void BoxItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (m_resizing) {
#ifdef ALTERNATIVE_RESIZING
		qreal dx = event->pos().x() - event->lastPos().x();
		qreal dy = event->pos().y() - event->lastPos().y();
		setRect(rect().adjusted(0, 0, dx, dy).normalized());
#else
		QRectF rectangle(rect());
		if (event->pos().x() < rectangle.x())
			rectangle.setBottomLeft(event->pos());
		else
			rectangle.setBottomRight(event->pos());
		setRect(rectangle);
#endif
		scene()->update();
	}
	else
		QGraphicsRectItem::mouseMoveEvent(event);
}

void BoxItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (m_resizing) {
		m_resizing = false;
		setCursor(Qt::ArrowCursor);
		emit dirty();
	}
	else
		QGraphicsRectItem::mouseReleaseEvent(event);
}

void BoxItem::updateTransform()
{
	QTransform transform;
	transform.shear(m_shearHorizontal, m_shearVertical);
	transform.rotate(m_angle);
	setTransform(transform);
}

QDataStream& operator<<(QDataStream& out, const BoxItem& boxItem)
{
	out << boxItem.pos() << boxItem.angle()
		<< boxItem.shearHorizontal() << boxItem.shearVertical()
		<< boxItem.zValue() << boxItem.rect() << boxItem.pen()
		<< boxItem.brush();
	return out;
}

QDataStream& operator>>(QDataStream& in, BoxItem& boxItem)
{
	QPointF position;
	double angle;
	double shearHorizontal;
	double shearVertical;
	double z;
	QRectF rect;
	QPen pen;
	QBrush brush;
	in >> position >> angle >> shearHorizontal >> shearVertical >> z
		>> rect >> pen >> brush;
	boxItem.setPos(position);
	boxItem.setAngle(angle);
	boxItem.setShear(shearHorizontal, shearVertical);
	boxItem.setZValue(z);
	boxItem.setRect(rect);
	boxItem.setPen(pen);
	boxItem.setBrush(brush);
	return in;
}
