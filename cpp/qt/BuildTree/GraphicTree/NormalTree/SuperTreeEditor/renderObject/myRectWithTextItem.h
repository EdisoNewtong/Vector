#ifndef MY_RECT_WITH_TEXT_ITEM
#define MY_RECT_WITH_TEXT_ITEM

#include <QGraphicsObject>

// class QGraphicsRectItem;
class myRect;
class QGraphicsTextItem;


class myRectWithTextItem : public QGraphicsObject
{
    Q_OBJECT
    // Q_PROPERTY(QRectF geometry READ geometry WRITE setGeometry)

public:
    // QGraphicsObject(QGraphicsItem *parent = nullptr);
    myRectWithTextItem(QGraphicsItem *parent = nullptr);
    virtual  ~myRectWithTextItem();

    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    // QGraphicsRectItem* getRectItem();
    myRect* getRectItem();
    QGraphicsTextItem* getTextItem();
protected:
    // QGraphicsRectItem*  m_pRectItem;
    myRect*  m_pRectItem;
    QGraphicsTextItem*  m_pTextItem;
};


#endif


