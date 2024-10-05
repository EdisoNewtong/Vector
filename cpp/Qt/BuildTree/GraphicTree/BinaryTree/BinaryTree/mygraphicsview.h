#pragma once

#include <QGraphicsView>
#include <QPoint>

class MyGraphicsView : public QGraphicsView
{
	Q_OBJECT
	Q_PROPERTY(QPoint hvValue READ getScrollValue WRITE setScrollValue)

public:
	MyGraphicsView(QWidget* parent = nullptr);
	virtual ~MyGraphicsView();

	QPoint getScrollValue();
	void setScrollValue(const QPoint& pt);

protected:
	QPoint m_hvValue;
};