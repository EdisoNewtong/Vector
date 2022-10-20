#pragma once

#include <QObject>
#include <QGraphicsEllipseItem>

class mygraphicscircleitem : public QObject , public QGraphicsEllipseItem
{
    Q_OBJECT
	Q_PROPERTY(QRectF rect READ rect WRITE setRect)
public:
	mygraphicscircleitem(QObject *parent = nullptr);

};