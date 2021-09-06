#include "TreeView.h"
#include "TreeModel.h"


#include <QDebug>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QModelIndex>
#include <QContextMenuEvent>

#include <QMessageBox>


TreeView::TreeView(QWidget* parent /* = nullptr*/)
    : QTreeView(parent)
    , m_pPopupMenu( nullptr )
    , m_pCreateChildNodeAtBeginAct( nullptr )
    , m_pCreateChildNodeAtEndAct( nullptr )
    , m_pInsertSiblingNodeBeforeSelectedNodeAct( nullptr )
    , m_pInsertSiblingNodeAfterSelectedNodeAct( nullptr )
    , m_pDeleteNodeAct( nullptr )
    , m_pDeleteNodesChildrenAct( nullptr )
    , m_pMoveUpAct( nullptr )
    , m_pMoveDownAct( nullptr )
    , m_pCreateParentWithChildAct( nullptr )
    , m_pPopupMenu2( nullptr )
    , m_pCreateRootNodeAct( nullptr )
    , m_RightClickSelectedValidIdx()
{
    //
    // Menu Parts
    //
    m_pPopupMenu = new QMenu(this);

    m_pCreateChildNodeAtBeginAct              = m_pPopupMenu->addAction( QStringLiteral("Prepend Child") ); 
    m_pCreateChildNodeAtEndAct                = m_pPopupMenu->addAction( QStringLiteral("Append  Child") );
    m_pInsertSiblingNodeBeforeSelectedNodeAct = m_pPopupMenu->addAction( QStringLiteral("Insert Sibling Node | Before") );
    m_pInsertSiblingNodeAfterSelectedNodeAct  = m_pPopupMenu->addAction( QStringLiteral("Insert Sibling Node | After") );

    m_pPopupMenu->addSeparator();

    m_pDeleteNodeAct                          = m_pPopupMenu->addAction( QStringLiteral("Delete Selected Node") );
    m_pDeleteNodesChildrenAct                 = m_pPopupMenu->addAction( QStringLiteral("Delete Selected Node's children") );

    m_pPopupMenu->addSeparator();

    m_pMoveUpAct                              = m_pPopupMenu->addAction( QStringLiteral("Move Up") );
    m_pMoveDownAct                            = m_pPopupMenu->addAction( QStringLiteral("Move Down") );

    m_pPopupMenu->addSeparator();

    m_pCreateParentWithChildAct               = m_pPopupMenu->addAction( QStringLiteral("Create New parent with current child") );
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //
    // connect actions with slot
    //
    connect( m_pCreateChildNodeAtBeginAct,   SIGNAL(triggered(bool)), this, SLOT( onPrependNodeActTrigger()) );
    connect( m_pCreateChildNodeAtEndAct,     SIGNAL(triggered(bool)), this, SLOT( onAppendNodeActTrigger()) );
    connect( m_pInsertSiblingNodeBeforeSelectedNodeAct, SIGNAL(triggered(bool)), this, SLOT( onInsertSiblingNodeBeforeActTrigger()) );
    connect( m_pInsertSiblingNodeAfterSelectedNodeAct,  SIGNAL(triggered(bool)), this, SLOT( onInsertSiblingNodeAfterActTrigger()) );
    connect( m_pDeleteNodeAct,  SIGNAL(triggered(bool)), this, SLOT( onDeleteNodeTrigger()) );
    connect( m_pDeleteNodesChildrenAct,  SIGNAL(triggered(bool)), this, SLOT( onDeleteNodesChildrenTrigger()) );
    connect( m_pMoveUpAct,      SIGNAL(triggered(bool)),  this, SLOT( onMoveUpActTrigger()) );  
    connect( m_pMoveDownAct,    SIGNAL(triggered(bool)),  this, SLOT( onMoveDownActTrigger()) );  
    connect( m_pCreateParentWithChildAct,    SIGNAL(triggered(bool)),  this, SLOT( onCreateParentWithChildActTrigger()) );  


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Menu 2
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    m_pPopupMenu2 = new QMenu(this);
    m_pCreateRootNodeAct = m_pPopupMenu2->addAction( QStringLiteral("Create Root Node") );
    connect( m_pCreateRootNodeAct,    SIGNAL(triggered(bool)),  this, SLOT( onCreateRootNodeActTrigger()) );
}


// virtual 
TreeView::~TreeView()
{

}



// virtual 
void TreeView::contextMenuEvent(QContextMenuEvent* event) // Q_DECL_OVERRIDE;
{
    QTreeView::contextMenuEvent( event );

    auto bSelectedFlag = hasOneSelectedItem(m_RightClickSelectedValidIdx);
    if ( !bSelectedFlag ) {
        auto model = this->model();
        TreeModel* treeModel = nullptr;
        if (   model != nullptr 
               // &&  ( treeModel = dynamic_cast<TreeModel*>(model) ) != nullptr )
               &&  ( treeModel = qobject_cast<TreeModel*>(model) ) != nullptr && !treeModel->hasRootNode() )
        {
            m_pPopupMenu2->popup( event->globalPos() );
            m_pPopupMenu2->setFocus( Qt::PopupFocusReason );
        }
        return;
    }


    if ( !bSelectedFlag || !m_RightClickSelectedValidIdx.isValid() || m_RightClickSelectedValidIdx.column() != 0 ) {
        return;
    }

    auto selRect = this->visualRect( m_RightClickSelectedValidIdx );
    auto isCursorInside = selRect.isValid() && selRect.contains( event->pos() );

    if ( !isCursorInside  ) {
        return;
    }

    qDebug() << "Updating QMenu Action ... ";
    ////////////////////////////////////////////////////////////////
    m_pCreateChildNodeAtBeginAct->setEnabled( true );
    m_pCreateChildNodeAtEndAct->setEnabled( true );

    auto parentIdx = m_RightClickSelectedValidIdx.parent();
    auto isRootNode = !parentIdx.isValid();

    m_pInsertSiblingNodeBeforeSelectedNodeAct->setEnabled( !isRootNode );
    m_pInsertSiblingNodeAfterSelectedNodeAct->setEnabled( !isRootNode );

    m_pDeleteNodeAct->setEnabled( !isRootNode );

    auto currentNode = static_cast<TreeNode*>( m_RightClickSelectedValidIdx.internalPointer() );
    m_pDeleteNodesChildrenAct->setEnabled( currentNode!=nullptr &&  currentNode->hasChildren() );
    
    m_pMoveUpAct->setEnabled( currentNode!=nullptr   && ( currentNode->canSwapWithPreviousNode()  != nullptr) );
    m_pMoveDownAct->setEnabled( currentNode!=nullptr && ( currentNode->canSwapWithFormerNode()    != nullptr) );

    // m_pCreateParentWithChildAct->setEnabled( !isRootNode );
    m_pCreateParentWithChildAct->setEnabled( true );
    ///////////////////////////////////////////////////////

    // else : is Valid 
    m_pPopupMenu->popup( event->globalPos() );
    m_pPopupMenu->setFocus( Qt::PopupFocusReason );
}


// slot
void TreeView::onPrependNodeActTrigger()
{
    // qDebug() << "TODO : Prepend Child";
    
    if ( !m_RightClickSelectedValidIdx.isValid() ) {
        return;
    }

    auto model = this->model();
    TreeModel* treeModel = nullptr;
    if (   model != nullptr 
           // &&  ( treeModel = dynamic_cast<TreeModel*>(model) ) != nullptr )
           &&  ( treeModel = qobject_cast<TreeModel*>(model) ) != nullptr )
    {
        auto bret = treeModel->prependChildNode( m_RightClickSelectedValidIdx );
        if ( bret ) {
            expand(m_RightClickSelectedValidIdx);
        }
    }
}

// slot
void TreeView::onAppendNodeActTrigger()
{
    // qDebug() << "TODO : Append Child";

    if ( !m_RightClickSelectedValidIdx.isValid() ) {
        return;
    }

    auto model = this->model();
    TreeModel* treeModel = nullptr;
    if (   model != nullptr 
           // &&  ( treeModel = dynamic_cast<TreeModel*>(model) ) != nullptr )
           &&  ( treeModel = qobject_cast<TreeModel*>(model) ) != nullptr )
    {
        auto bret = treeModel->appendChildNode( m_RightClickSelectedValidIdx );
        if ( bret ) {
            expand(m_RightClickSelectedValidIdx);
        }
    }
}

// slot
void TreeView::onInsertSiblingNodeBeforeActTrigger()
{
    // qDebug() << "TODO : Insert Sibling Child | Before ";
    
    if ( !m_RightClickSelectedValidIdx.isValid() ) {
        return;
    }

    auto model = this->model();
    TreeModel* treeModel = nullptr;
    if (   model != nullptr 
           // &&  ( treeModel = dynamic_cast<TreeModel*>(model) ) != nullptr )
           &&  ( treeModel = qobject_cast<TreeModel*>(model) ) != nullptr )
    {
        treeModel->insertSiblingNodeBefore( m_RightClickSelectedValidIdx );
    }
}

// slot
void TreeView::onInsertSiblingNodeAfterActTrigger()
{
    // qDebug() << "TODO : Insert Sibling Child | After ";

    if ( !m_RightClickSelectedValidIdx.isValid() ) {
        return;
    }

    auto model = this->model();
    TreeModel* treeModel = nullptr;
    if (   model != nullptr 
           // &&  ( treeModel = dynamic_cast<TreeModel*>(model) ) != nullptr )
           &&  ( treeModel = qobject_cast<TreeModel*>(model) ) != nullptr )
    {
        treeModel->insertSiblingNodeAfter( m_RightClickSelectedValidIdx );
    }
}

// slot
void TreeView::onDeleteNodeTrigger()
{
    // qDebug() << "TODO : Delete Node ";
    if ( !m_RightClickSelectedValidIdx.isValid() ) {
        return;
    }

    auto model = this->model();
    TreeModel* treeModel = nullptr;
    if (   model != nullptr 
           // &&  ( treeModel = dynamic_cast<TreeModel*>(model) ) != nullptr )
           &&  ( treeModel = qobject_cast<TreeModel*>(model) ) != nullptr )
    {
        int bNeedReleaseMemoryFlag = 1;
        treeModel->deleteSelectedNode(m_RightClickSelectedValidIdx, bNeedReleaseMemoryFlag);
    }
}

// slot
void TreeView::onDeleteNodesChildrenTrigger()
{
    if ( !m_RightClickSelectedValidIdx.isValid() ) {
        return;
    }

    auto model = this->model();
    TreeModel* treeModel = nullptr;
    if (   model != nullptr 
           // &&  ( treeModel = dynamic_cast<TreeModel*>(model) ) != nullptr )
           &&  ( treeModel = qobject_cast<TreeModel*>(model) ) != nullptr )
    {
        // int bNeedReleaseMemoryFlag = 1;
        // treeModel->deleteSelectedNode(m_RightClickSelectedValidIdx, bNeedReleaseMemoryFlag);
        auto parentNode = static_cast<TreeNode*>( m_RightClickSelectedValidIdx.internalPointer() );
        if ( parentNode!=nullptr && parentNode->hasChildren() ) {
            int bNeedReleaseMemoryFlag = 1;
            auto bret = treeModel->deleteSelectedNodesChildren( m_RightClickSelectedValidIdx, bNeedReleaseMemoryFlag);

            if ( bret ) {
                collapse(m_RightClickSelectedValidIdx);
            }
        }
    }
}


// slot
void TreeView::onMoveUpActTrigger()
{
    // qDebug() << "TODO : Move Node Up";
    if ( !m_RightClickSelectedValidIdx.isValid() ) {
        return;
    }

    auto model = this->model();
    TreeModel* treeModel = nullptr;
    if (   model != nullptr 
           // &&  ( treeModel = dynamic_cast<TreeModel*>(model) ) != nullptr )
           &&  ( treeModel = qobject_cast<TreeModel*>(model) ) != nullptr )
    {
        auto bret = treeModel->swapUp( m_RightClickSelectedValidIdx );
        qDebug() << "swapUp : " << (bret ? " SUCC" : " Failed");
    }
}

// slot
void TreeView::onMoveDownActTrigger()
{
    qDebug() << "TODO : Move Node Down";
    if ( !m_RightClickSelectedValidIdx.isValid() ) {
        return;
    }

    auto model = this->model();
    TreeModel* treeModel = nullptr;
    if (   model != nullptr 
           // &&  ( treeModel = dynamic_cast<TreeModel*>(model) ) != nullptr )
           &&  ( treeModel = qobject_cast<TreeModel*>(model) ) != nullptr )
    {
        auto bret = treeModel->swapDown( m_RightClickSelectedValidIdx );
        qDebug() << "swapDown : " << (bret ? " SUCC" : " Failed");
    }
}

// slot
void TreeView::onCreateParentWithChildActTrigger()
{
    // qDebug() << "TODO : Create Parent With Child";
    if ( !m_RightClickSelectedValidIdx.isValid() ) {
        return;
    }

    auto model = this->model();
    TreeModel* treeModel = nullptr;
    if (   model != nullptr 
           // &&  ( treeModel = dynamic_cast<TreeModel*>(model) ) != nullptr )
           &&  ( treeModel = qobject_cast<TreeModel*>(model) ) != nullptr )
    {
        QModelIndex newParentIdx;
        auto bret = treeModel->createParentWithChild(m_RightClickSelectedValidIdx, newParentIdx);
        if ( bret && newParentIdx.isValid() ) {
            expand(newParentIdx);
        }
    }

}


bool TreeView::hasOneSelectedItem(QModelIndex& selectedIdx)
{
    auto model_idx_list = this->selectedIndexes();
    if ( model_idx_list.empty() ) {
        return false;
    }

    // assign Selected ModelIndex to reference
    selectedIdx = model_idx_list.at(0);
    return true;
}


void TreeView::onCreateRootNodeActTrigger()
{
    // qDebug() << "Creating Root Node";
    auto model = this->model();
    TreeModel* treeModel = nullptr;
    if (   model != nullptr
           // &&  ( treeModel = dynamic_cast<TreeModel*>(model) ) != nullptr )
           &&  ( treeModel = qobject_cast<TreeModel*>(model) ) != nullptr )
    {
        auto b = treeModel->createRootNode();
        if ( !b ) {
            // ui->statusBar->showMessage(errorMsg, 3500);
            QMessageBox::critical(this,QStringLiteral("Error"), QStringLiteral("Sorry : Generate Root Node Failed"));
        }
    }
}

