#pragma once

#include <QVector>
#include <QtWidgets/QWidget>

#include "ui_timeschedule.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TimeScheduleClass; };
QT_END_NAMESPACE

class Range
{
public:
	Range();
	Range(int min, int max);

	void setMin(int min);
	void setMax(int max);

	int min() const;
	int max() const;
	int rangeValue() const;
	
	int isToMaxDiff(int value);
	int isToMinDiff(int value);

	bool contains(int value) const;
	bool intersected(const Range& other) const;
	Range united(const Range& other) const;

	bool operator<(const Range& other) const;

private:
	int		m_min = 0;
	int		m_max = 0;
};

class QPainter;
class TimeSchedule : public QWidget
{
	enum OpType
	{
		OP_NONE = 0,
		OP_DRAW,
		OP_SELECT,
		OP_RESIZE_MIN,
		OP_RESIZE_MAX
	};

    Q_OBJECT

public:
    TimeSchedule(QWidget *parent = nullptr);
    ~TimeSchedule();

	void setBarColor(const QColor& color);
	void setBackgroundColor(const QColor& color);
	void setForegroundColor(const QColor& color);

	void setMainScaleMarkLenth(int len);
	void setSubScaleMarkLenth(int len);

	//0~24*60
	void addRange(int min, int max);
	void clear();

	QVector<QPair<int, int>> getTimeDurations();

protected:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
	void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;

	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
	void drawBackground(QPainter* painter);
	void drawTimeRuler(QPainter* painter);
	void drawTimeBar(QPainter* painter);
	void drawOpRangeText(QPainter* painter);

	void updateHoverRangeText(int posX);

	void addNewRange(const Range& range);

	int getLeftBoundary(int w);
	int getRightBoundary(int w);
	int canvasBeginPos();
	int canvasEndPos();
	int canvasLength();

	void range2Time(const Range& range, int& beginHour, int& beginMinue, int& endHour, int& endMinute);
	void time2Range(int beginHour, int beginMinue, int endHour, int endMinute, Range& range);
	QString range2TimeText(const Range& range);

	//
	int position2TimeValue(int pos);
	int timeValue2Position(int minute);
	int timeValue2Position(int hour, int minute);

	int timeValue2Pixels(int minute);
	int timeValue2Pixels(int hour, int minute);

private:
    Ui::TimeScheduleClass *ui;

	QColor			m_barColor;
	QColor			m_fgColor;
	QColor			m_bgColor;

	int				m_mainScaleMarkLen = 10;
	int				m_subScaleMarkLen = 5;

	//int				m_pressedPosX = 0;
	//int				m_lastPosX = 0;

	int				m_pressedTimeX = 0;
	int				m_lastTimeX = 0;

	OpType			m_opType = OP_NONE;
	Range			m_opRange;
	
	QString			m_rangeText;
	Range			m_hoverRange;
	QVector<Range>	m_ranges;
};
