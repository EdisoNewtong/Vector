#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <QString>
#include <QList>

//
// Tree Node Data Only
//
class TreeNode
{
public:
	TreeNode(const QString& text, TreeNode* parent);
	virtual ~TreeNode();
	
	// create node
	TreeNode* appendChild();
	TreeNode* prependChild();
	TreeNode* insertSiblingNodeBefore();
	TreeNode* insertSiblingNodeAfter();


	TreeNode* getParent();
	int       selfIndex();
	TreeNode* getChild(int idx);

	QString   getText();
	void      setText(const QString& txt);
	int       childCount();
protected:
	//
	// Data Field
	//
	QString   m_text;
	TreeNode* m_parent;
	QList<TreeNode*> m_children;

	// DisplayInfo m_displayOption;
};

#endif
