#ifndef MY_RECT_WITH_TEXT_ITEM
#define MY_RECT_WITH_TEXT_ITEM

#include <QGraphicsObject>
#include <QSizeF>

class QGraphicsRectItem;
class QGraphicsTextItem;


class myRectWithTextItem : public QGraphicsObject
{
    Q_OBJECT
    // Q_PROPERTY(QRectF geometry READ geometry WRITE setGeometry)

public:
    myRectWithTextItem(QGraphicsItem *parent, int layer, int childIdx);
    virtual  ~myRectWithTextItem();

    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;


    //
    // Set Rect property
    //
    void setRectBrush( const QBrush& brush);
    void setRectPen( const QPen& pen);
    void setRectBrushAndPen( const QBrush& brush, const QPen& pen);
    void setRectInfo( const QRectF rect);
    void setRectPos(const QPointF rect);

    //
    // Set Text property
    //
    void setTextColor( const QColor& color);
    void setTextFont( const QFont& font);
    void setTextFontAndColor( const QFont& font, const QColor& color);
    void setText( const QString& txt);
    void setTextPos( const QPointF pos);
    void moveTextToCenter();

    int getLayer();
    int getSelfIdx();

    QSizeF getRectSize();
protected:
    QGraphicsRectItem*  m_pRectItem;
    QGraphicsTextItem*  m_pTextItem;

    int m_layer;  // start from 0
    int m_childIdx; // start from 0
};


#endif


