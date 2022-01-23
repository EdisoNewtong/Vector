#include "xmlStandardItem.h"

#include "rapidxml.hpp"
#include <QDebug>

// static int G_CNT = 0;

xmlStandardItem::xmlStandardItem(const QString& text, rapidxml::xml_node<char>* node)
    : QStandardItem(text)
    , m_pXmlNode(node)
{
    // qDebug() << "Create Item : " << ++G_CNT;
    // setBackground( QBrush(Qt::red) );
//    if ( node ) {
//        if ( node->type() == rapidxml::node_element  ) {
//            setBackground( QBrush(Qt::red) );
//        } else if ( node->type() == rapidxml::node_data ) {
//            setBackground( QBrush(Qt::green) );
//        }
//    }
}

// virtual
xmlStandardItem::~xmlStandardItem()
{
    // qDebug() << "Delete Item : " << --G_CNT;
}


rapidxml::xml_node<char>* xmlStandardItem::getXmlNode()
{
    return m_pXmlNode;
}
