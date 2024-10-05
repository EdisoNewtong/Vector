#ifndef GRAPHIC_NODE_H
#define GRAPHIC_NODE_H

#include <QList>
#include <QRect>
#include <QSize>

class QStandardItem;


class GraphicNode
{
public:
    GraphicNode(int level, QStandardItem* item);
    GraphicNode();
    virtual ~GraphicNode();

    void updateSize(); // final update
    QSize getSize();
    void appendChild(GraphicNode* child);

protected:
    int                    m_level;
    QStandardItem*        m_qtItem;
    QList<GraphicNode*> m_children;
    QRect            m_renderShape;
};

#endif
