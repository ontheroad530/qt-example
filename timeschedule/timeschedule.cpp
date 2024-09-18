#include "timeschedule.h"

#include <QMenu>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>

#define BOUNDARY_ERR 5
#define SCALE_COUNT 24

#define SCALE_ACCURACY (60*24)

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

void TimeSchedule::setBackgroundColor(const QColor& color)
{
	m_bgColor = color;
}

void TimeSchedule::setForegroundColor(const QColor& color)
{
	m_fgColor = color;
}

void TimeSchedule::setMainScaleMarkLenth(int len)
{
	m_mainScaleMarkLen = len;
}

void TimeSchedule::setSubScaleMarkLenth(int len)
{
	m_subScaleMarkLen = len;
}

void TimeSchedule::addRange(int min, int max)
{
	m_ranges << Range(min, max);

	if (this->isVisible())
	{
		update();
	}
}

void TimeSchedule::clear()
{
	m_ranges.clear();
	update();
}

QVector<QPair<int, int>> TimeSchedule::getTimeDurations()
{
	QVector<QPair<int, int>> timeDurations;
	
	for (const auto& range : m_ranges)
	{
		int min = range.min();
		int max = range.max();
		timeDurations << qMakePair<int, int>(min, max);
	}

	return timeDurations;
}

void TimeSchedule::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	drawBackground(&painter);
	drawTimeBar(&painter);
	drawTimeRuler(&painter);
	drawOpRangeText(&painter);
}

void TimeSchedule::leaveEvent(QEvent* event)
{
	m_rangeText.clear();
	update();

	QWidget::leaveEvent(event);
}

void TimeSchedule::mousePressEvent(QMouseEvent* event)
{
	int posX = event->pos().x();
	if (posX >= canvasBeginPos() && posX <= canvasEndPos())
	{
		m_opType = OP_DRAW;

		int time = position2TimeValue(posX);
		m_opRange = Range(time, time);
		m_pressedTimeX = time;
		m_lastTimeX = m_pressedTimeX;
		for (int i = 0; i < m_ranges.size(); ++i)
		{
			if (m_ranges[i].isToMaxDiff(time) <= BOUNDARY_ERR)
			{
				m_opRange = m_ranges[i];
				m_opType = OP_RESIZE_MAX;
				m_ranges.removeAt(i);
				this->setCursor(Qt::SizeHorCursor);
				break;
			}
			else if (m_ranges[i].isToMinDiff(time) <= BOUNDARY_ERR)
			{
				m_opRange = m_ranges[i];
				m_opType = OP_RESIZE_MIN;
				m_ranges.removeAt(i);
				this->setCursor(Qt::SizeHorCursor);
				break;
			}
			else if (m_ranges[i].contains(time))
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
	addNewRange(m_opRange);
	m_opType = OP_NONE;
	this->unsetCursor();
	update();

	QWidget::mouseReleaseEvent(event);
}

void TimeSchedule::mouseMoveEvent(QMouseEvent* event)
{
	int posX = event->pos().x();
	if (posX >= canvasBeginPos() && posX <= canvasEndPos())
	{
		int time = position2TimeValue(posX);
		if (OP_DRAW == m_opType)
		{
			m_opRange.setMin(m_pressedTimeX);
			m_opRange.setMax(qMax(m_pressedTimeX, time));
			update();
		}
		else if (OP_SELECT == m_opType)
		{
			int move = time - m_lastTimeX;
			int min = m_opRange.min() + move;
			int max = m_opRange.max() + move;


			if (min >= 0 && max <= SCALE_ACCURACY)
			{
				m_opRange.setMin(min);
				m_opRange.setMax(max);
			}

			m_lastTimeX = time;
			update();
		}
		else if (OP_RESIZE_MIN == m_opType)
		{
			m_opRange.setMin(qMin(time, m_opRange.max()));
			this->setCursor(Qt::SizeHorCursor);
			update();
		}
		else if (OP_RESIZE_MAX == m_opType)
		{
			m_opRange.setMax(qMax(time, m_opRange.min()));
			this->setCursor(Qt::SizeHorCursor);
			update();
		}
		else if (OP_NONE == m_opType)
		{
			bool showSizeCursor = false;
			for (int i = 0; i < m_ranges.size(); ++i)
			{
				if (m_ranges[i].isToMaxDiff(time) <= BOUNDARY_ERR
					|| m_ranges[i].isToMinDiff(time) <= BOUNDARY_ERR)
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

		updateHoverRangeText(time);
	}
	else
	{
		this->unsetCursor();
	}
	
	QWidget::mouseMoveEvent(event);
}

void TimeSchedule::drawBackground(QPainter* painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(m_bgColor);

	QRect r = rect();
	r.setLeft(canvasBeginPos());
	r.setRight(canvasEndPos());
	painter->drawRect(r);
	painter->restore();
}

void TimeSchedule::drawTimeRuler(QPainter* painter)
{
	int numScales = SCALE_COUNT;
	painter->save();
	painter->setPen(m_fgColor);

	for (int i = 0; i < numScales+1; ++i)
	{
		int posX = canvasBeginPos() + this->canvasLength() / numScales * i;
		if (i % 2 == 0){
			int posY = this->height() - m_mainScaleMarkLen;
			painter->drawLine(posX, posY, posX, this->height());

			QString text = QString("%1:00").arg(i, 2, 10, QChar('0'));
			int width = this->fontMetrics().width(text);
			int height = this->height() - m_mainScaleMarkLen;
			painter->drawText(posX-width/2, height, text);
		}else{
			int posY = this->height() - m_subScaleMarkLen;
			painter->drawLine(posX, posY, posX, this->height());
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
		int x1 = timeValue2Position(range.min());
		int x2 = timeValue2Position(range.max());

		QRect rect;
		rect.setLeft(x1);
		rect.setRight(x2);
		rect.setTop(0);
		rect.setBottom(this->height());
		painter->drawRect(rect);
	}

	if (OP_NONE != m_opType)
	{
		int x1 = timeValue2Position(m_opRange.min());
		int x2 = timeValue2Position(m_opRange.max());

		QRect rect;
		rect.setLeft(x1);
		rect.setRight(x2);
		rect.setTop(0);
		rect.setBottom(this->height());

		painter->drawRect(rect);
	}

	painter->restore();
}

void TimeSchedule::drawOpRangeText(QPainter* painter)
{
	if (m_rangeText.isEmpty()){
		return;
	}

	int fontWidth = this->fontMetrics().width(m_rangeText);
	int fontHeight = this->fontMetrics().height();

	int posX = timeValue2Position(m_hoverRange.min());
	int pixels = timeValue2Pixels(m_hoverRange.rangeValue());

	int x = posX + pixels / 2 - fontWidth / 2;
	int y = fontHeight;

	painter->save();
	painter->setPen(m_fgColor);
	painter->drawText(x, y, m_rangeText);
	painter->restore();
}

void TimeSchedule::updateHoverRangeText(int time)
{
	bool showText = false;
	m_rangeText.clear();
	if (OP_NONE == m_opType)
	{
		for (int i = 0; i < m_ranges.size(); ++i)
		{
			if (m_ranges[i].contains(time))
			{
				showText = true;
				m_hoverRange = m_ranges[i];
				break;
			}
		}
	}
	else
	{
		m_hoverRange = m_opRange;
		showText = true;
	}

	if (showText)
	{
		m_rangeText = range2TimeText(m_hoverRange);
	}

	update();
}

void TimeSchedule::addNewRange(const Range& range)
{
	Range r = range;
	for (auto iter = m_ranges.begin(); iter != m_ranges.end();)
	{
		if (r.intersected(*iter))
		{
			r = r.united(*iter);
			iter = m_ranges.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	m_ranges << r;

	//去除无效段
	for (auto iter = m_ranges.begin(); iter != m_ranges.end();)
	{
		if (iter->rangeValue() <= 0) {
			iter = m_ranges.erase(iter);
		}else {
			++iter;
		}
	}
}

int TimeSchedule::getLeftBoundary(int w)
{
	return w % SCALE_COUNT + SCALE_COUNT;
}

int TimeSchedule::getRightBoundary(int w)
{
	return  SCALE_COUNT;
}

int TimeSchedule::canvasBeginPos()
{
	return getLeftBoundary(this->width());
}

int TimeSchedule::canvasEndPos()
{
	return this->width() - getRightBoundary(this->width());
}

int TimeSchedule::canvasLength()
{
	return this->width() - getLeftBoundary(this->width()) - getRightBoundary(this->width());
}

void TimeSchedule::range2Time(const Range& range, int& beginHour, int& beginMinue, int& endHour, int& endMinute)
{
	beginHour = range.min() / 60;
	beginMinue = range.min() % 60;

	endHour = range.max() / 60;
	endMinute = range.max() % 60;
}

void TimeSchedule::time2Range(int beginHour, int beginMinue, int endHour, int endMinute, Range& range)
{
	range.setMin( beginHour * 60 + beginMinue );
	range.setMax( endHour * 60 + endMinute );
}

QString TimeSchedule::range2TimeText(const Range& range)
{
	int beginHour, beginMinue, endHour, endMinute;
	range2Time(range, beginHour, beginMinue, endHour, endMinute);
	return QString("%1:%2-%3:%4")
		.arg(beginHour, 2, 10, QChar('0')).arg(beginMinue, 2, 10, QChar('0'))
		.arg(endHour, 2, 10, QChar('0')).arg(endMinute, 2, 10, QChar('0'));
}

int TimeSchedule::position2TimeValue(int pos)
{
	int value = pos - canvasBeginPos();
	int hourPixels = canvasLength() / SCALE_COUNT;

	int hour = value / hourPixels;
	int minute = value % hourPixels * 60 / hourPixels;

	return hour * 60 + minute;
}

int TimeSchedule::timeValue2Position(int minute)
{
	return timeValue2Pixels(minute) + canvasBeginPos();
}

int TimeSchedule::timeValue2Position(int hour, int minute)
{
	return timeValue2Pixels(hour, minute) + canvasBeginPos();
}

int TimeSchedule::timeValue2Pixels(int minute)
{
	return timeValue2Pixels(minute / 60, minute % 60);
}

int TimeSchedule::timeValue2Pixels(int hour, int minute)
{
	int hourPixels = canvasLength() / SCALE_COUNT;
	int pixels = hour * hourPixels + minute * hourPixels / 60;
	return pixels;
}
