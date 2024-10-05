#include "myRectWithTextItem.h"

#include <QDebug>
// #include <QGraphicsRectItem>
// #include "myRect.h"
#include <QGraphicsTextItem>
#include <QPainter>

static int G_REF_CNT = 0;


myRectWithTextItem::myRectWithTextItem(QGraphicsItem *parent, int layer, int childIdx)
    : QGraphicsObject(parent)
    , m_pRectItem( nullptr )
    , m_pTextItem( nullptr )
    , m_layer( layer )
    , m_childIdx( childIdx )
{
    qDebug() << "myRectWithTextItem Create() ... " << ++G_REF_CNT;
    
    m_pRectItem = new QGraphicsRectItem(0,0, 200, 100, this);
    m_pRectItem->setPen( QPen( QBrush(Qt::red), 1.5));
    m_pRectItem->setBrush( Qt::black );


    m_pTextItem = new QGraphicsTextItem( QString(), m_pRectItem );
    m_pTextItem->setTextInteractionFlags( Qt::TextEditable );
    // m_pTextItem->setPos(50,50);
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



// QGraphicsRectItem* myRectWithTextItem::getRectItem()
// {
//     return m_pRectItem;
// }
// 
// QGraphicsTextItem* myRectWithTextItem::getTextItem()
// {
//     return m_pTextItem;
// }


void myRectWithTextItem::setRectBrush( const QBrush& brush)
{
    if ( m_pRectItem != nullptr ) {
        m_pRectItem->setBrush(brush);
    }
}

void myRectWithTextItem::setRectPen( const QPen& pen)
{
    if ( m_pRectItem != nullptr ) {
        m_pRectItem->setPen(pen);
    }
}

void myRectWithTextItem::setRectBrushAndPen( const QBrush& brush, const QPen& pen)
{
    if ( m_pRectItem != nullptr ) {
        m_pRectItem->setBrush(brush);
        m_pRectItem->setPen(pen);
    }
}

void myRectWithTextItem::setRectInfo( const QRectF rect)
{
    if ( m_pRectItem != nullptr ) {
        m_pRectItem->setRect( rect );
    }
    
}

void myRectWithTextItem::setRectPos(const QPointF pos)
{
    if ( m_pRectItem != nullptr ) {
        m_pRectItem->setPos( pos );
    }
}




void myRectWithTextItem::setTextColor( const QColor& color)
{
    if ( m_pTextItem != nullptr ) {
        m_pTextItem->setDefaultTextColor( color );
    }
}

void myRectWithTextItem::setTextFont( const QFont& font)
{
    if ( m_pTextItem != nullptr ) {
        m_pTextItem->setFont( font );
    }
}

void myRectWithTextItem::setTextFontAndColor( const QFont& font, const QColor& color)
{
    if ( m_pTextItem != nullptr ) {
        m_pTextItem->setFont( font );
        m_pTextItem->setDefaultTextColor( color );
    }
}

void myRectWithTextItem::setText( const QString& txt)
{
    if ( m_pTextItem != nullptr ) {
        m_pTextItem->setPlainText( txt );
    }
}

void myRectWithTextItem::setTextPos( const QPointF pos)
{
    if ( m_pTextItem != nullptr ) {
        m_pTextItem->setPos( pos );
    }
}


int myRectWithTextItem::getLayer()
{
    return m_layer;
}

int myRectWithTextItem::getSelfIdx()
{
    return m_childIdx;
}


void myRectWithTextItem::moveTextToCenter()
{
    if ( m_pRectItem != nullptr && m_pTextItem != nullptr ) {
        auto rect = m_pRectItem->rect();
        auto centerPt = rect.center();
        auto textRect = m_pTextItem->boundingRect();

        m_pTextItem->setPos( centerPt.x() - textRect.width() / 2.0 , centerPt.y() - textRect.height() / 2.0 );
    }
}


QSizeF myRectWithTextItem::getRectSize()
{
    if ( m_pRectItem != nullptr ) {
        return m_pRectItem->rect().size();
    } else { 
        return QSizeF();
    }
}

