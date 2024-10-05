#include "myDirFileItem.h"

#include <QDebug>

static int G_ITEM_CNT = 0;

myDirFileItem::myDirFileItem(myDirFileItem* parent,  myDirFileItem::E_ITEM_TYPE type, const QString& content)
    : m_parent( parent )
    , m_type( type )
    , m_content( content )
{
    qDebug() << ++G_ITEM_CNT << " item created. " << endl;
}



// virtual 
myDirFileItem::~myDirFileItem()
{
    int id = G_ITEM_CNT;
    qDebug() << id << " item deleted. Remain = " << --G_ITEM_CNT << endl;
}


myDirFileItem* myDirFileItem::getParent() const
{
    return m_parent;
}


myDirFileItem::E_ITEM_TYPE myDirFileItem::getType() const
{
    return m_type;
}


QString myDirFileItem::getContent() const
{
    return m_content;
}







