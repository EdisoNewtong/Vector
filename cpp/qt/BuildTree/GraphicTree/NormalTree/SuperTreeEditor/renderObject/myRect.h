#ifndef MY_RECT_H
#define MY_RECT_H

#include <QGraphicsRectItem>

class myRect : public QGraphicsRectItem
{
public:
    // QGraphicsRectItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr)
    myRect(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr);
    ~myRect();
};

#endif

