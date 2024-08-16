#include "timeschedule.h"

#include <QDebug>
#include <QPainter>
#include <QMouseEvent>

#define BOUNDARY_ERR 5
#define CANVAS_BOUND 20
#define SCALE_COUNT 24

Range::Range()
{
}

Range::Range(int min, int max)
	:m_min(min), m_max(max)
{
}

void Range::setMin(int min)
{
	m_min = min;
}

void Range::setMax(int max)
{
	m_max = max;
}

int Range::min() const
{ 
	return m_min; 
}

int Range::max() const 
{ 
	return m_max; 
}

int Range::rangeValue() const
{
	return m_max - m_min;
}

int Range::isToMaxDiff(int value)
{
	return qAbs(m_max - value);
}

int Range::isToMinDiff(int value)
{
	return qAbs(m_min - value);
}

bool Range::contains(int value) const
{
	return value >= m_min && value <= m_max;
}

bool Range::intersected(const Range& other) const
{
	if (this->min() > other.max() || this->max() < other.min())
	{
		return false;
	}

	return true;
}

Range Range::united(const Range& other) const
{
	return Range(qMin(this->min(), other.min()), qMax(this->max(), other.max()));
}

bool Range::operator<(const Range& other) const
{
	return this->min() < other.min();
}

TimeSchedule::TimeSchedule(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimeScheduleClass())
{
    ui->setupUi(this);

	this->setMouseTracking(true);
}

TimeSchedule::~TimeSchedule()
{
    delete ui;
}

void TimeSchedule::setBarColor(const QColor& color)
{
	m_barColor = color;
}

void TimeSchedule::setFontColor(const QColor& color)
{
	m_fontColor = color;
}

void TimeSchedule::setMainScaleMarkLenth(int len)
{
	m_mainScaleMarkLen = len;
}

void TimeSchedule::setSubScaleMarkLenth(int len)
{
	m_subScaleMarkLen = len;
}

void TimeSchedule::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	drawTimeBar(&painter);
	drawTimeRuler(&painter);
	drawOpRangeText(&painter);
}

void TimeSchedule::resizeEvent(QResizeEvent* event)
{
	int oldBoundary = getBoundary(event->oldSize().width());
	int newBoundary = getBoundary(event->size().width());

	int oldWidth = event->oldSize().width() - 2 * oldBoundary;
	int newWidth = event->size().width() - 2 * newBoundary;

	for (auto& range : m_ranges)
	{
		range.setMin((range.min() - oldBoundary) * newWidth / oldWidth + newBoundary);
		range.setMax((range.max()- oldBoundary)  * newWidth / oldWidth + newBoundary);
	}

	QWidget::resizeEvent(event);
}

void TimeSchedule::mousePressEvent(QMouseEvent* event)
{
	int posX = event->pos().x();
	if (posX >= canvasBeginPos() && posX <= canvasEndPos())
	{
		m_opType = OP_DRAW;
		m_pressedPosX = posX;
		m_lastPosX = m_pressedPosX;
		for (int i = 0; i < m_ranges.size(); ++i)
		{
			if (m_ranges[i].isToMaxDiff(posX) < BOUNDARY_ERR)
			{
				m_opRange = m_ranges[i];
				m_opType = OP_RESIZE_MAX;
				m_ranges.removeAt(i);
				this->setCursor(Qt::SizeHorCursor);
				break;
			}
			else if (m_ranges[i].isToMinDiff(posX) < BOUNDARY_ERR)
			{
				m_opRange = m_ranges[i];
				m_opType = OP_RESIZE_MIN;
				m_ranges.removeAt(i);
				this->setCursor(Qt::SizeHorCursor);
				break;
			}
			else if (m_ranges[i].contains(posX))
			{
				m_opRange = m_ranges[i];
				m_opType = OP_SELECT;
				m_ranges.removeAt(i);
				this->setCursor(Qt::OpenHandCursor);
				update();
				break;
			}
		}
	}

	QWidget::mousePressEvent(event);
}

void TimeSchedule::mouseReleaseEvent(QMouseEvent* event)
{
	m_ranges << m_opRange;
	m_opType = OP_NONE;
	this->unsetCursor();

	mergeRanges();
	update();

	QWidget::mouseReleaseEvent(event);
}

void TimeSchedule::mouseMoveEvent(QMouseEvent* event)
{
	int posX = event->pos().x();
	if (posX >= canvasBeginPos() && posX <= canvasEndPos())
	{
		if (OP_DRAW == m_opType)
		{
			m_opRange.setMin(m_pressedPosX);
			m_opRange.setMax(qMax(m_pressedPosX, posX));
			update();
		}
		else if (OP_SELECT == m_opType)
		{
			int move = posX - m_lastPosX;
			int min = m_opRange.min() + move;
			int max = m_opRange.max() + move;
			if (min >= canvasBeginPos() && max <= canvasEndPos())
			{
				m_opRange.setMin(min);
				m_opRange.setMax(max);
			}

			m_lastPosX = posX;
			update();
		}
		else if (OP_RESIZE_MIN == m_opType)
		{
			m_opRange.setMin(qMin(posX, m_opRange.max()));
			update();
		}
		else if (OP_RESIZE_MAX == m_opType)
		{
			m_opRange.setMax(qMax(posX, m_opRange.min()));
			update();
		}
		else if (OP_NONE == m_opType)
		{
			bool showSizeCursor = false;
			for (int i = 0; i < m_ranges.size(); ++i)
			{
				if (m_ranges[i].isToMaxDiff(posX) < BOUNDARY_ERR
					|| m_ranges[i].isToMinDiff(posX) < BOUNDARY_ERR)
				{
					showSizeCursor = true;
					
					break;
				}
			}

			if (showSizeCursor){
				this->setCursor(Qt::SizeHorCursor);
			}else{
				this->unsetCursor();
			}
		}
	}
	
	QWidget::mouseMoveEvent(event);
}

void TimeSchedule::drawTimeRuler(QPainter* painter)
{
	int numScales = SCALE_COUNT;
	painter->save();
	painter->setPen(m_fontColor);

	for (int i = 0; i < numScales+1; ++i)
	{
		int posX = canvasBeginPos() + this->canvasLength() / numScales * i;
		if (i % 2 == 0){
			painter->drawLine(posX, 0, posX, m_mainScaleMarkLen);

			QString text = QString("%1:00").arg(i, 2, 10, QChar('0'));
			int width = this->fontMetrics().width(text);
			int height = this->fontMetrics().height() + m_mainScaleMarkLen;
			painter->drawText(posX-width/2, height, text);
		}else{
			painter->drawLine(posX, 0, posX, m_subScaleMarkLen);
		}
	}

	painter->restore();
}

void TimeSchedule::drawTimeBar(QPainter* painter)
{
	painter->save();
	painter->setBrush(m_barColor);
	painter->setPen(m_barColor);

	//绘制未改变的区域
	for (const auto& range : m_ranges)
	{
		painter->drawRect(range.min(), 0, range.rangeValue(), this->height());
	}

	if (OP_NONE != m_opType)
	{
		painter->drawRect(m_opRange.min(), 0, m_opRange.rangeValue(), this->height());
	}

	painter->restore();
}

void TimeSchedule::drawOpRangeText(QPainter* painter)
{
	if (OP_NONE == m_opType){
		return;
	}

	int minValue = m_opRange.min() - canvasBeginPos();
	int maxValue = m_opRange.max() - canvasBeginPos();
	int scacleInterval = (qreal)canvasLength() / (qreal)SCALE_COUNT;

	QString beginTime = QString("%1:%2")
		.arg(minValue / scacleInterval, 2, 10, QChar('0'))
		.arg(minValue % scacleInterval * 60 / scacleInterval, 2, 10, QChar('0'));

	QString endTime = QString("%1:%2")
		.arg(maxValue / scacleInterval, 2, 10, QChar('0'))
		.arg(maxValue % scacleInterval * 60 / scacleInterval, 2, 10, QChar('0'));

	QString text = QString("%1--%2").arg(beginTime).arg(endTime);


	int fontWidth = this->fontMetrics().width(text);

	int x = canvasBeginPos() + (canvasLength() + fontWidth)/ 2 ;
	int y = this->height() - 5;

	painter->save();
	painter->setPen(m_fontColor);
	painter->drawText(x, y, text);
	painter->restore();
}

void TimeSchedule::mergeRanges()
{
	//去重
	QSet<int> valueSet;
	for (const auto& range : m_ranges)
	{
		valueSet << range.min();
		valueSet << range.max();
	}

	//排序
	QList<int> list = valueSet.toList();
	qSort(list);

	//合并
	QVector<Range> newRanges;
	for (int i = 0; i < m_ranges.size(); ++i)
	{
		if (i + 1 >= m_ranges.size())
		{
			newRanges << m_ranges[i];
			break;
		}
		else
		{
			if (m_ranges[i].intersected(m_ranges[i + 1]))
			{
				m_ranges[i + 1] = m_ranges[i].united(m_ranges[i + 1]);
			}
			else
			{
				newRanges << m_ranges[i];
			}
		}
	}

	m_ranges = newRanges;
}

int TimeSchedule::getBoundary(int w)
{
	return w % SCALE_COUNT + CANVAS_BOUND;
}

int TimeSchedule::canvasBeginPos()
{
	return getBoundary(this->width());
}

int TimeSchedule::canvasEndPos()
{
	return this->width() - getBoundary(this->width());
}

int TimeSchedule::canvasLength()
{
	return this->width() - 2 * getBoundary(this->width());
}
