#include "treenode.h"

#include <QDebug>

static int G_NODE_CNT = 0;

treenode::treenode(const QString& text, treenode* parent, bool isInvisibleRoot /* = false*/)
	: m_text(text)
	, m_LeftNode( nullptr )
	, m_RightNode( nullptr )
	, m_parentNode( parent )
	, m_graphicsRenderObject( nullptr )
	, m_depth( 0 )
	, m_height( 0 )
	, m_layerIdx( 0 )
	, m_centerPt_x(0.0)
	, m_centerPt_y(0.0)
	, m_isInvisibleRoot( isInvisibleRoot )
{
	qDebug() << "Alloc " << ++G_NODE_CNT << " self-defined treenode";
}


treenode::~treenode()
{
	qDebug() << "Delete " << --G_NODE_CNT << " self-defined treenode";

	//
	// delete Left && Right child
	//
	deleteLeftNode();
	deleteRightNode();

}



// Left Part


treenode* treenode::leftNode()
{
	return m_LeftNode;
}


treenode* treenode::addLeftNode( const QString& leftText)
{
	if ( m_LeftNode == nullptr ) {
		m_LeftNode = new treenode( leftText, this);
	}
	return m_LeftNode;
}


void treenode::deleteLeftNode()
{
	if ( m_LeftNode != nullptr ) {
		if ( m_LeftNode ->m_parentNode != nullptr ) {
			// detach self from parent
			m_LeftNode->m_parentNode = nullptr;
		}

		delete m_LeftNode;
		m_LeftNode = nullptr;
	}
}




// Right Part
treenode* treenode::rightNode()
{
	return m_RightNode;
}

treenode* treenode::addRightNode(const QString& rightText)
{
    if ( m_RightNode == nullptr ) {
		m_RightNode = new treenode( rightText, this);
	}
	return m_RightNode;
}

void treenode::deleteRightNode()
{
	if ( m_RightNode != nullptr ) {
		if ( m_RightNode->m_parentNode != nullptr ) {
			// detach self from parent
			m_RightNode->m_parentNode = nullptr;
		}

		delete m_RightNode;
		m_RightNode = nullptr;
	}
}




treenode* treenode::parent()
{
	return m_parentNode;
}


void treenode::setText(const QString& text)
{
	m_text = text;
}


QString treenode::text()
{
	return m_text;
}




void treenode::attachRenderObject(QGraphicsItem* item)
{
	m_graphicsRenderObject = item;
}


void treenode::detachRenderObject()
{
	m_graphicsRenderObject = nullptr;
}


bool treenode::isRoot()
{
	return m_parentNode != nullptr && m_parentNode->isInvisibleRoot();
}

QPair<int,treenode::E_ChildType> treenode::getChildState()
{
	int childCnt = 0;
    treenode::E_ChildType tp = E_NONE;

	if ( m_LeftNode!=nullptr && m_RightNode!=nullptr ) {
		childCnt = 2;
		tp = E_BOTH;
	} else if ( m_LeftNode!=nullptr || m_RightNode!=nullptr ) {
		childCnt = 1;
        tp = ( m_LeftNode!=nullptr ?  E_LEFT : E_RIGHT);
	} else {
		childCnt = 0;
        tp = E_NONE;
	}

    return qMakePair(childCnt,tp);
	
}

int treenode::depth()    { return m_depth; }
int treenode::height()   { return m_height; }
int treenode::layerIdx() { return m_layerIdx; }
double treenode::x()     { return m_centerPt_x; }
double treenode::y()     { return m_centerPt_y; }

void treenode::setDepth(int depth)   { m_depth = depth; }
void treenode::setHeight(int height) { m_height = height; }
void treenode::setLayerIdx(int idx)  { m_layerIdx = idx; }
void treenode::setCenterPt_x(double x)     { m_centerPt_x = x; }
void treenode::setCenterPt_y(double y)     { m_centerPt_y = y; }
