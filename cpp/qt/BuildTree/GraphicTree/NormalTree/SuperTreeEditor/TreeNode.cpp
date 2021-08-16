#include "TreeNode.h"
#include <QDebug>

static bool G_ENABLE_REFCOUNT = true;
static int G_CNT = 0;

TreeNode::TreeNode(const QString& text, const QString& val,TreeNode* parent)
    : m_name(text)
    , m_value(val)
    , m_parent(parent)
{
    m_children.clear();
    if ( G_ENABLE_REFCOUNT ) {
        qDebug() << "Create G_CNT = " << ++G_CNT;
    }
}



// virtual
TreeNode::~TreeNode()
{
    releaseSelfAndChildren();
}

void      TreeNode::releaseSelfAndChildren()
{
    if ( m_parent != nullptr ) {
        int foundIdx = m_parent->m_children.indexOf( this );
        if ( foundIdx != -1 ) {
            m_parent->m_children.removeAt(foundIdx);
        }
    }

    //
    // Delete children
    //
    for ( auto it = m_children.begin(); it != m_children.end(); ++it ) {
        auto node = *it;
        if ( node != nullptr ) {
            node->m_parent = nullptr;
        }

        delete node;
        *it = nullptr;
    }
    m_children.clear();
    // delete this;


    /*
    if ( m_parent != nullptr ) {
        if ( !m_parent->m_children.isEmpty() ) {
            int foundIdx = m_parent->m_children.indexOf( this );
            if ( foundIdx != -1 ) {
                // delete self inside the children list
                m_parent->m_children.removeAt(foundIdx);

                // cut parent relationship
                m_parent = nullptr;
            }
        }
    } else {
        //
        // this node is the <Root> Node , delete children is enough
        //
        for ( auto it = m_children.begin(); it != m_children.end(); ++it ) {
            auto node = *it;
            if ( node !=nullptr ) {
                node->m_parent = nullptr;
                delete node;
            }
            *it = nullptr;
        }
        m_children.clear();
    }
    */

    if ( G_ENABLE_REFCOUNT ) {
        qDebug() << "Delete G_CNT = " << --G_CNT;
    }
}


//
// insert at tail
//
TreeNode* TreeNode::appendChild()
{
    TreeNode* newCreateNode = new TreeNode( QString(""), QString(""), this);
    m_children.push_back(newCreateNode);
    return newCreateNode;
}

//
// insert at head
//
TreeNode* TreeNode::prependChild()
{
    TreeNode* newCreateNode = new TreeNode( QString(""), QString(""), this);
    m_children.push_front(newCreateNode);
    return newCreateNode;
}


/*
TreeNode* TreeNode::insertSiblingNodeBefore()
{
    if ( m_parent != nullptr ) {
        if ( !m_parent->m_children.isEmpty() ) {
            int foundIdx = m_parent->m_children.indexOf( this );
            if ( foundIdx != -1 ) {
                TreeNode* newCreateNode = new TreeNode( QString(""), QString(""), this->m_parent);
                m_parent->m_children.insert(foundIdx, newCreateNode);
                return newCreateNode;
            }
        }
    }

    return nullptr;
}

TreeNode* TreeNode::insertSiblingNodeAfter()
{
    if ( m_parent != nullptr ) {
        if ( !m_parent->m_children.isEmpty() ) {
            int foundIdx = m_parent->m_children.indexOf( this );
            if ( foundIdx != -1 ) {
                TreeNode* newCreateNode = new TreeNode( QString(""), QString(""), this->m_parent);
                int insertPos = foundIdx + 1;
                if ( insertPos >= m_parent->m_children.size() ) {
                    m_parent->m_children.push_back( newCreateNode);
                } else {
                    m_parent->m_children.insert(insertPos, newCreateNode);
                }
                return newCreateNode;
            }
        }
    }

    return nullptr;
}
*/

TreeNode* TreeNode::getParent()
{
    return m_parent;
}

// void TreeNode::setParent(TreeNode* parent)
// {
//     m_parent = parent;
// }


int       TreeNode::selfIndex()
{
    if ( m_parent != nullptr ) {
        return m_parent->m_children.indexOf( this );
    }

    return 0;
}

QString TreeNode::getName()
{
    return m_name;
}

void TreeNode::setName(const QString& name)
{
    m_name = name;
}



QString   TreeNode::getValue()
{
    return m_value;
}

void      TreeNode::setValue(const QString& val)
{
    m_value = val;
}


void      TreeNode::removeFromParent()
{
    if ( m_parent != nullptr ) {
        int foundIdx = m_parent->m_children.indexOf( this );
        if ( foundIdx != -1 ) {
            // delete self inside the children list
            m_parent->m_children.removeAt(foundIdx);

            // broken ownership relationship
            // if ( brokeRelationship ) {
                m_parent = nullptr;
            // }
        }
    }
}


int TreeNode::childCount()
{
    return m_children.size();
}

TreeNode* TreeNode::getChildAtIdx(int idx)
{
    if ( idx >=0 && idx < m_children.size() ) {
        return m_children.at(idx);
    }

    return nullptr;
}



TreeNode* TreeNode::insertNodeAtIndex(int idx)
{
    if ( idx < 0 || idx > childCount() ) {
        return nullptr;
    }

    TreeNode* newInsertNode = new TreeNode( QString(""), QString(""), this);
    m_children.insert(idx, newInsertNode);
    return newInsertNode;
}




TreeNode* TreeNode::pushExistedChild(TreeNode* child)
{
    child->m_parent = this;
    m_children.push_back(child);
    return child;
}

TreeNode* TreeNode::insertExistedChild(TreeNode* child, int idx)
{
    child->m_parent = this;
    m_children.insert(idx, child);
    return child;
}

bool  TreeNode::hasChildren()
{
    return !m_children.empty();
}


TreeNode*  TreeNode::canSwapWithPreviousNode()
{
    int selfIdx = selfIndex();
    if( selfIdx > 0 ) {
        return m_parent->m_children.at( selfIdx - 1 );
    } else {
        return nullptr;
    }
}


// void  TreeNode::swapWithPreviousNode()
// {
//     int selfIdx = selfIndex();
//     m_parent->m_children.swapItemsAt( selfIdx, selfIdx - 1 );
// }


TreeNode*  TreeNode::canSwapWithFormerNode()
{
    int selfIdx = selfIndex();
    int childrenCnt = m_parent->childCount();
    if ( selfIdx != (childrenCnt - 1) ) {
        return m_parent->m_children.at( selfIdx + 1);
    } else {
        return nullptr;
    }
}


// void  TreeNode::swapWithFormerNode()
// {
//     int selfIdx = selfIndex();
//     // Must have Former Node
//     m_parent->m_children.swapItemsAt( selfIdx, selfIdx + 1 );
// }




