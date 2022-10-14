#ifndef BINARY_TREE_VIEW_H
#define BINARY_TREE_VIEW_H

#include <QTreeView>
#include <QModelIndex>

#include "treenode.h"

class QMenu;
class QAction;
class QContextMenuEvent;
class QStandardItem;



namespace rapidxml {
    template<class T>  class xml_document;
    template<class T>  class xml_node;
}

class binarytreeview : public QTreeView
{
    Q_OBJECT

public:
    explicit binarytreeview(QWidget* parent = nullptr);
    virtual ~binarytreeview();


	void initEventHandler();

    QPair<QModelIndex,treenode*> getSelectedNodeItem(int* pColInfo);
public slots:
    void onDeleteNode();
    void onAdd2NodesActionTrigger();
    void onAddLeftNodeActionTrigger();
    void onAddRightNodeActionTrigger();

signals:
	void addLeftNode(const QModelIndex& rightClickParentNode);
	void addRightNode(const QModelIndex& rightClickParentNode);
	void addBothNodes(const QModelIndex& rightClickParentNode);
	void deleteNode(const QModelIndex& rightClickCurrentNode);

protected:
    void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;

    QMenu*      m_pPopupMenu;

    QAction*    m_pDelteNodeAct; // delete node
    QAction*    m_pAddBothAct;   // add both left And right nodes
    QAction*    m_pAddLeftAct;   // add only left node
    QAction*    m_pAddRightAct;  // add only right node


};

#endif