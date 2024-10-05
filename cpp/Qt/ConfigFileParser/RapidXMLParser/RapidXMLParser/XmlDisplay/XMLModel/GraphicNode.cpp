#include "GraphicNode.h"

#include <QStandardItem>


static const int SHAPE_WIDTH  = 50;
static const int SHAPE_HEIGHT = 20;
static const int SHAPE_H_GAP = 7;
static const int SHAPE_V_GAP = 4;


GraphicNode::GraphicNode(int level, QStandardItem* item)
    : m_level(level)
    , m_qtItem(item)
    , m_renderShape(0,0,SHAPE_WIDTH,SHAPE_HEIGHT)
{
    m_children.clear();
}

GraphicNode::~GraphicNode()
{
    m_qtItem = nullptr;
    for ( auto& pChild : m_children ) {
        delete pChild;
        pChild = nullptr;
    }
    m_children.clear();
}


void GraphicNode::updateSize()
{
    int w = 0;
    int childCnt = 0;
    for ( auto& pChild : m_children ) {
        if ( pChild!=nullptr ) {
            w += pChild->getSize().width();
        }
        ++childCnt;
    }

    if ( childCnt == 0 ) {
        w = SHAPE_WIDTH; // set as self width
    } else {
        // > 0
        w += (childCnt-1) * SHAPE_H_GAP;
    }

    m_renderShape.setWidth(w);
}


QSize GraphicNode::getSize()
{
    return m_renderShape.size();
}


void GraphicNode::appendChild(GraphicNode* child)
{
    m_children.push_back(child);
}


