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
    TreeNode(const QString& name, const QString& val, TreeNode* parent);
    virtual ~TreeNode();

    // create node
    TreeNode* appendChild();
    TreeNode* prependChild();
    TreeNode* insertSiblingNodeBefore();
    TreeNode* insertSiblingNodeAfter();

    TreeNode* insertNodeAtIndex(int idx);

    TreeNode* getParent();
    int       selfIndex();
    TreeNode* getChild(int idx);

    QString   getName();
    void      setName(const QString& name);

    QString   getValue();
    void      setValue(const QString& val);

    void      removeFromParent();

    int       childCount();

    void      releaseSelfAndChildren();
protected:
    //
    // Data Field
    //
    QString   m_name;
    QString   m_value;

    TreeNode* m_parent;
    QList<TreeNode*> m_children;

    // DisplayInfo m_displayOption;
};

#endif
