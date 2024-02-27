#include "textitem.h"
#include "global.hpp"
#include "textitemdialog.h"

#include <QGraphicsView>

TextItem::TextItem(const QPoint& position, QGraphicsScene* scene)
	: QGraphicsTextItem(), m_angle(0.0), m_shearHorizontal(0.0),
	m_shearVertical(0.0)
{
	setFont(QFont("Helvetica", 11));
	setFlags(QGraphicsItem::ItemIsSelectable |
#if QT_VERSION >= 0x040600
		QGraphicsItem::ItemSendsGeometryChanges |
#endif
		QGraphicsItem::ItemIsMovable);
	setPos(position);
	scene->clearSelection();
	scene->addItem(this);
	setSelected(true);
}

TextItem::~TextItem()
{}

void TextItem::setAngle(double angle)
{
	if (isSelected() && !qFuzzyCompare(m_angle, angle)) {
		m_angle = angle;
		updateTransform();
	}
}

void TextItem::setShear(double shearHorizontal, double shearVertical)
{
	if (isSelected() &&
		(!qFuzzyCompare(m_shearHorizontal, shearHorizontal) ||
			!qFuzzyCompare(m_shearVertical, shearVertical))) {
		m_shearHorizontal = shearHorizontal;
		m_shearVertical = shearVertical;
		updateTransform();
	}
}

void TextItem::edit()
{
	QWidget* window = 0;
	QList<QGraphicsView*> views = scene()->views();
	if (!views.isEmpty())
		window = views.at(0)->window();
	TextItemDialog dialog(this, QPoint(), scene(), window);
	if (dialog.exec())
		emit dirty();
}

QVariant TextItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (isDirtyChange(change))
		emit dirty();
	return QGraphicsTextItem::itemChange(change, value);
}

void TextItem::updateTransform()
{
	QTransform transform;
	transform.shear(m_shearHorizontal, m_shearVertical);
	transform.rotate(m_angle);
	setTransform(transform);
}

QDataStream& operator<<(QDataStream& out, const TextItem& textItem)
{
	out << textItem.pos() << textItem.angle()
		<< textItem.shearHorizontal() << textItem.shearVertical()
		<< textItem.zValue() << textItem.toHtml();
	return out;
}

QDataStream& operator>>(QDataStream& in, TextItem& textItem)
{
	QPointF position;
	double angle;
	double shearHorizontal;
	double shearVertical;
	double z;
	QString html;
	in >> position >> angle >> shearHorizontal >> shearVertical >> z
		>> html;
	textItem.setPos(position);
	textItem.setAngle(angle);
	textItem.setShear(shearHorizontal, shearVertical);
	textItem.setZValue(z);
	textItem.setHtml(html);
	return in;
}
