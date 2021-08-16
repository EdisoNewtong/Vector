#ifndef Tree_View_H
#define Tree_View_H

#include <QTreeView>

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    TreeView(QWidget* parent = nullptr);
    virtual ~TreeView();

protected:
    // override right click menu
    virtual void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;

    bool hasOneSelectedItem(QModelIndex& selectedIdx);
private slots:
    void onPrependNodeActTrigger();
    void onAppendNodeActTrigger();
    void onInsertSiblingNodeBeforeActTrigger();
    void onInsertSiblingNodeAfterActTrigger();

    void onDeleteNodeTrigger();
    void onDeleteNodesChildrenTrigger();

    void onMoveUpActTrigger();
    void onMoveDownActTrigger();

    void onCreateParentWithChildActTrigger();

protected:
    //
    // Menu and Menu Items
    // 
    QMenu*      m_pPopupMenu;

    QAction*    m_pCreateChildNodeAtBeginAct;
    QAction*    m_pCreateChildNodeAtEndAct;
    QAction*    m_pInsertSiblingNodeBeforeSelectedNodeAct;
    QAction*    m_pInsertSiblingNodeAfterSelectedNodeAct;

    QAction*    m_pDeleteNodeAct;
    QAction*    m_pDeleteNodesChildrenAct;

    QAction*    m_pMoveUpAct;
    QAction*    m_pMoveDownAct;

    QAction*    m_pCreateParentWithChildAct;
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    QModelIndex m_RightClickSelectedValidIdx;
};

#endif
