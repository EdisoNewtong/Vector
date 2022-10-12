#pragma once

#include <QString>
#include <QGraphicsItem>

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

	QGraphicsItem* m_graphicsRenderObject;

	int       m_depth;
	int       m_height;


	bool      m_isInvisibleRoot;
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
	bool isInvisibleRoot() { return m_isInvisibleRoot; }

	QPair<int,treenode::E_ChildType> getChildState();

	int depth();
	int height();
	void setDepth(int depth);
	void setHeight(int height);

};

