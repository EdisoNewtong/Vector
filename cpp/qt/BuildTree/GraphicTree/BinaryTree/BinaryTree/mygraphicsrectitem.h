#pragma once

#include <QObject>
#include <QGraphicsRectItem>

class mygraphicsrectitem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
	mygraphicsrectitem(QObject *parent = nullptr);

};
