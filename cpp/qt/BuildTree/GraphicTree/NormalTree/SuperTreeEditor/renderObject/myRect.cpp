#include "myRect.h"

#include <QDebug>

myRect::myRect(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent /* = nullptr*/)
    : QGraphicsRectItem(x,y,  width,  height, parent )
{
    qDebug() << "myRect Create";
}

myRect::~myRect()
{
    qDebug() << "myRect Delete";
}


