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
	void setFontColor(const QColor& color);

	void setMainScaleMarkLenth(int len);
	void setSubScaleMarkLenth(int len);

protected:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
	void drawTimeRuler(QPainter* painter);
	void drawTimeBar(QPainter* painter);
	void drawOpRangeText(QPainter* painter);

	void mergeRanges();

	int getBoundary(int w);
	int canvasBeginPos();
	int canvasEndPos();
	int canvasLength();

private:
    Ui::TimeScheduleClass *ui;

	QColor			m_barColor;
	QColor			m_fontColor;

	int				m_mainScaleMarkLen = 10;
	int				m_subScaleMarkLen = 5;

	int				m_pressedPosX = 0;
	int				m_lastPosX = 0;

	OpType			m_opType = OP_NONE;
	Range			m_opRange;
	QVector<Range>	m_ranges;
};
