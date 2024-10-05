#pragma once

#include <QString>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>    // Round Item
#include <QGraphicsLineItem>       // Line Item
#include <QGraphicsSimpleTextItem> // Text Item

#include "nodestylecfg.h"

class treenode 
{
public:
	enum E_ChildType {
		E_NONE  = 0,
		E_LEFT  = 1,
		E_RIGHT = 2,
		E_BOTH  = 3
	};
	treenode(const QString& text, treenode* parent, bool isInvisibleRoot = false);
	virtual ~treenode();

protected:
	QString   m_text;
	treenode* m_LeftNode;
	treenode* m_RightNode;
	treenode* m_parentNode;
	bool      m_selectedState;

	QGraphicsEllipseItem*       m_pGraphicCircle;
	QGraphicsLineItem*          m_pGraphicLine;
	QGraphicsSimpleTextItem*    m_pGraphicText;

	int       m_depth;
	int       m_height;
	int       m_layerIdx;

	int       m_absDepth;
	int       m_absHeight;
	int       m_absLayerIdx;

	double    m_centerPt_x;
	double    m_centerPt_y;

	double    m_connectionLineSelf_x;
	double    m_connectionLineSelf_y;
	double    m_connectionLineParent_x;
	double    m_connectionLineParent_y;

	bool      m_isInvisibleRoot;

	nodeStyleCfg m_nodeStyle;
public:
	void setText(const QString& text);
	QString text();

	treenode* leftNode();
	treenode* addLeftNode( const QString& leftText);
	void deleteLeftNode();

	treenode* rightNode();
	treenode* addRightNode( const QString& leftText);
	void deleteRightNode();

	treenode* parent();

	void attachRenderObject(QGraphicsItem* item);
	void detachRenderObject();

	bool isRoot();
	bool isInvisibleRoot(); 
	bool isLeftNode();

	QPair<int,treenode::E_ChildType> getChildState();

	int depth();
	int height();
	int layerIdx();

	int absDepth();
	int absHeight();
	int absLayerIdx();

	double x();
	double y();

	double connectionLineSelfDot_x();
	double connectionLineSelfDot_y();
	double connectionLineParent_x();
	double connectionLineParent_y();



	void setDepth(int depth);
	void setHeight(int height);
	void setLayerIdx(int idx);

	void setAbsDepth(int depth);
	void setAbsHeight(int height);
	void setAbsLayerIdx(int idx);

	void setCenterPt_x(double x);
	void setCenterPt_y(double y);


	void setConnectionLine_SelfDot(double x, double y);
	void setConnectionLine_ParentDot(double x, double y); 

	QGraphicsEllipseItem*     circleObject();
	QGraphicsLineItem*        lineObject();
	QGraphicsSimpleTextItem*  textObject();

	void setCircle(QGraphicsEllipseItem* circle);
	void setLine(QGraphicsLineItem* line);
	void setTextObject(QGraphicsSimpleTextItem* textObj);
	void detachAllRenderObject();

	nodeStyleCfg getNodeStyle();
	void         setNodeStyle( const nodeStyleCfg& newCfg);


	bool isSelected();
	void select();
	void unselect();
};