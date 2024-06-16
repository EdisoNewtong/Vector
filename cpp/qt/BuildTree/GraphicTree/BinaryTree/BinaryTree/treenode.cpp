#include "treenode.h"

#include <QDebug>

static int G_NODE_CNT = 0;
static bool G_TRACE_ALLOC_RELEASE_FLAG = false;

treenode::treenode(const QString& text, treenode* parent, bool isInvisibleRoot /* = false*/)
	: m_text(text)
	, m_LeftNode( nullptr )
	, m_RightNode( nullptr )
	, m_parentNode( parent )
	, m_selectedState( false )
	, m_pGraphicCircle( nullptr )
	, m_pGraphicLine( nullptr )
	, m_pGraphicText( nullptr )
	, m_depth( 0 )
	, m_height( 0 )
	, m_layerIdx( 0 )
	, m_absDepth( 0 )
	, m_absHeight( 0 )
	, m_absLayerIdx( 0 )
	, m_centerPt_x(0.0)
	, m_centerPt_y(0.0)
	, m_connectionLineSelf_x( 0.0 )
	, m_connectionLineSelf_y( 0.0 )
	, m_connectionLineParent_x( 0.0 )
	, m_connectionLineParent_y( 0.0 )
	, m_isInvisibleRoot( isInvisibleRoot )
	, m_nodeStyle( nodeStyleCfg::DefaultCfg() )
{
    if ( G_TRACE_ALLOC_RELEASE_FLAG ) {
        qDebug() << "Alloc " << ++G_NODE_CNT << " self-defined treenode";
    }
}


treenode::~treenode()
{
    if ( G_TRACE_ALLOC_RELEASE_FLAG ) {
        qDebug() << "Delete " << --G_NODE_CNT << " self-defined treenode";
    }

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


void treenode::setCircle(QGraphicsEllipseItem* circle)
{
	m_pGraphicCircle = circle; 
}


QGraphicsEllipseItem* treenode::circleObject()
{
	return m_pGraphicCircle;
}

void treenode::setLine(QGraphicsLineItem* line)
{
	m_pGraphicLine = line;
}

QGraphicsLineItem* treenode::lineObject()
{
	return m_pGraphicLine;
}


void treenode::setTextObject(QGraphicsSimpleTextItem* textObj)
{
	m_pGraphicText = textObj;
}

QGraphicsSimpleTextItem* treenode::textObject()
{
	return m_pGraphicText;
}


void treenode::detachAllRenderObject()
{
	setCircle(nullptr);
	setLine(nullptr);
	setTextObject(nullptr);
}




bool treenode::isRoot()
{
	return m_parentNode != nullptr && m_parentNode->isInvisibleRoot();
}


bool treenode::isInvisibleRoot() { return m_isInvisibleRoot; }

bool treenode::isLeftNode()
{
	if ( isRoot() ) {
		return true;
	}

	auto bIsLeft = false;
	auto p = parent();
	if ( p != nullptr ) {
		bIsLeft = (p->leftNode() == this);
	} 

    return bIsLeft;
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

int treenode::absDepth()    { return m_absDepth; }
int treenode::absHeight()   { return m_absHeight; }
int treenode::absLayerIdx() { return m_absLayerIdx; }

double treenode::x()     { return m_centerPt_x; }
double treenode::y()     { return m_centerPt_y; }
double treenode::connectionLineSelfDot_x()    { return m_connectionLineSelf_x; }
double treenode::connectionLineSelfDot_y()    { return m_connectionLineSelf_y; }
double treenode::connectionLineParent_x() { return m_connectionLineParent_x; }
double treenode::connectionLineParent_y() { return m_connectionLineParent_y; }



nodeStyleCfg treenode::getNodeStyle() { return m_nodeStyle; }

void treenode::setDepth(int depth)   { m_depth = depth; }
void treenode::setHeight(int height) { m_height = height; }
void treenode::setLayerIdx(int idx)  { m_layerIdx = idx; }

void treenode::setAbsDepth(int depth)   { m_absDepth = depth; }
void treenode::setAbsHeight(int height) { m_absHeight = height; }
void treenode::setAbsLayerIdx(int idx)  { m_absLayerIdx = idx; }


void treenode::setCenterPt_x(double x)     { m_centerPt_x = x; }
void treenode::setCenterPt_y(double y)     { m_centerPt_y = y; }
void treenode::setConnectionLine_SelfDot(double x, double y)   { m_connectionLineSelf_x   = x; m_connectionLineSelf_y   = y; }
void treenode::setConnectionLine_ParentDot(double x, double y) { m_connectionLineParent_x = x; m_connectionLineParent_y = y; }


void treenode::setNodeStyle(const nodeStyleCfg& newCfg) { m_nodeStyle = newCfg; }


bool treenode::isSelected() { return m_selectedState; }
void treenode::select()   { m_selectedState = true; }
void treenode::unselect() { m_selectedState = false; }


