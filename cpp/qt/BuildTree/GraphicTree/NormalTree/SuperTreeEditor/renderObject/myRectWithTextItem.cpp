#include "myRectWithTextItem.h"

#include <QDebug>
// #include <QGraphicsRectItem>
#include "myRect.h"
#include <QGraphicsTextItem>
#include <QPainter>

static int G_REF_CNT = 0;


myRectWithTextItem::myRectWithTextItem(QGraphicsItem *parent /* = nullptr*/)
    : QGraphicsObject(parent)
    , m_pRectItem( nullptr )
    , m_pTextItem( nullptr )
{
    qDebug() << "myRectWithTextItem Create() ... " << ++G_REF_CNT;
    
    m_pRectItem = new myRect(20,20, 200, 100, this);
    m_pRectItem->setPen( QPen( QBrush(Qt::red), 2.0));
    // m_pRectItem->setBrush( Qt::blue );

    m_pTextItem = new QGraphicsTextItem("Text", this );
    m_pTextItem->setTextInteractionFlags( Qt::TextEditable );
    m_pTextItem->setPos(50,50);
    m_pTextItem->setDefaultTextColor( Qt::green );
}

// virtual  
myRectWithTextItem::~myRectWithTextItem()
{
    qDebug() << "myRectWithTextItem Destroy() ... " << --G_REF_CNT;
}


// virtual 
QRectF myRectWithTextItem::boundingRect() const // override
{
    // qreal penWidth = 1;
    // return QRectF(-10 - penWidth / 2, -10 - penWidth / 2,
    //               20 + penWidth, 20 + penWidth);

    return m_pRectItem->rect();
}

// virtual 
void myRectWithTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
           QWidget *widget) // override
{
    (void)option;
    (void)widget;

    auto rect = m_pRectItem->rect();
    // rect.setX( rect.x() + 50);
    // rect.setY( rect.y() + 50);
    painter->drawRect( rect );
    // painter->drawRoundedRect(-10, -10, 20, 20, 5, 5);
}



myRect* myRectWithTextItem::getRectItem()
{
    return m_pRectItem;
}

QGraphicsTextItem* myRectWithTextItem::getTextItem()
{
    return m_pTextItem;
}



