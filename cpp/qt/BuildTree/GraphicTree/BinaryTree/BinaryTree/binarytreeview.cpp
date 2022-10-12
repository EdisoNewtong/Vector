#include "binarytreeview.h"

#include "binarytreemodel.h"

#include "globalSettings.h"

#include <QToolTip>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include <QStandardItem>
#include <QRect>
#include <QDebug>



// explicit
binarytreeview::binarytreeview(QWidget* parent /*= nullptr*/)
    : QTreeView(parent)
    , m_pPopupMenu(nullptr)
    , m_pDelteNodeAct(nullptr)
    , m_pAddBothAct(nullptr)
    , m_pAddLeftAct(nullptr)
    , m_pAddRightAct(nullptr)
{
    m_pPopupMenu = new QMenu(this);

    m_pAddBothAct = m_pPopupMenu->addAction("Add Left && Right Nodes");
    m_pAddLeftAct = m_pPopupMenu->addAction("Add Left Node");
    m_pAddRightAct = m_pPopupMenu->addAction("Add Right Node");
    m_pPopupMenu->addSeparator();
    m_pDelteNodeAct = m_pPopupMenu->addAction("Delete Select Node");

    connect( m_pAddBothAct,   SIGNAL(triggered(bool)), this, SLOT(onAdd2NodesActionTrigger()) );
    connect( m_pAddLeftAct,   SIGNAL(triggered(bool)), this, SLOT(onAddLeftNodeActionTrigger()) );
    connect( m_pAddRightAct,  SIGNAL(triggered(bool)), this, SLOT(onAddRightNodeActionTrigger()) );
    connect( m_pDelteNodeAct, SIGNAL(triggered(bool)), this, SLOT(onDeleteNode()) );


    //
    // Core Core Core
    // When you want to edit a grid, you need to follow 2 steps :
    //    1. Single-click a certain grid to select it
    //    2. Double-click it for edit the content inside the grid
    //
    // But when the 1st step, the delegate appearance doesn't work
    // The following code can let it work immediately
    setEditTriggers( QAbstractItemView::CurrentChanged );



}


// virtual 
binarytreeview::~binarytreeview()
{
	binarytreemodel* model = qobject_cast<binarytreemodel*>( this->model() );
	if ( model != nullptr ) {
		disconnect(this, SIGNAL(addLeftNode(const QModelIndex&)), model, SLOT(create_AddLeftNode(const QModelIndex&)) );
		disconnect(this, SIGNAL(addRightNode(const QModelIndex&)), model, SLOT(create_AddRightNode(const QModelIndex&)) );
        disconnect(this, SIGNAL(addBothNodes(const QModelIndex&)), model,  SLOT(create_BothNodes(const QModelIndex&)) );
		disconnect(this, SIGNAL(deleteNode(const QModelIndex&)), model, SLOT(delete_SelectedNode(const QModelIndex&)) );
	}

    auto selectionModel = this->selectionModel();
    this->setModel(nullptr);
    if ( selectionModel!=nullptr ) {
        delete selectionModel;
        selectionModel = nullptr;
    }
    
}


QPair<QModelIndex,treenode*> binarytreeview::getSelectedNodeItem(int* pColInfo)
{
    auto selLst = this->selectedIndexes();
    treenode* treeData = nullptr;
    if ( selLst.size() == 1 ) {
        auto selidx = selLst.at(0);
        QModelIndex retIdx = selidx;

        auto nCol = selidx.column();

        if ( pColInfo!=nullptr ) {
            *pColInfo = nCol;
        }

        if ( nCol!=0 ) {
            return qMakePair(QModelIndex(), nullptr);
        }
        
		treeData = static_cast<treenode*>( selidx.internalPointer() );
        return qMakePair(retIdx, treeData);
    }

    return qMakePair(QModelIndex(), treeData);
}

void binarytreeview::contextMenuEvent(QContextMenuEvent* event) // Q_DECL_OVERRIDE;
{
    QTreeView::contextMenuEvent(event);

    auto colInfo = -1;
    auto selitemPr = this->getSelectedNodeItem(&colInfo);
	auto modelidx = selitemPr.first;
	auto treePtr = selitemPr.second;
    if ( colInfo != 0 || !modelidx.isValid() || treePtr == nullptr ) {
        return;
    } 

    auto selRect = this->visualRect( modelidx );
    auto isCursorInside = selRect.isValid() && selRect.contains( event->pos() );

    if ( isCursorInside && treePtr!=nullptr ) {
        if ( treePtr!=nullptr ) {

			// set Context Menu Item for Deleting Operatation
            auto rootFlag = treePtr->isRoot();
            m_pDelteNodeAct->setEnabled(!rootFlag);

            auto childStatePr = treePtr->getChildState();
			auto childCnt = childStatePr.first;
            if ( childCnt == 2 ) {
                m_pAddBothAct->setEnabled(false);
                m_pAddLeftAct->setEnabled(false);
                m_pAddRightAct->setEnabled(false);
            } else if ( childCnt == 1 ) {
				m_pAddBothAct->setEnabled(false);
				auto childTag = childStatePr.second;
				if ( childTag == treenode::E_LEFT ) {
					// Only has Left Child
					m_pAddLeftAct->setEnabled(false);
					m_pAddRightAct->setEnabled(true);
				} else {
					// Only has Right Child
					m_pAddLeftAct->setEnabled(true);
					m_pAddRightAct->setEnabled(false);
				}
            } else if ( childCnt == 0 ) {
                m_pAddBothAct->setEnabled(true);
                m_pAddLeftAct->setEnabled(true);
                m_pAddRightAct->setEnabled(true);
            }
        }

        m_pPopupMenu->popup( event->globalPos() );
        m_pPopupMenu->setFocus(Qt::PopupFocusReason);
    }
}


void binarytreeview::onDeleteNode()
{
    auto colInfo = -1;
	auto selItemPr = this->getSelectedNodeItem(&colInfo);
	auto modelIdx = selItemPr.first;
	auto treePtr = selItemPr.second;
    
    if ( colInfo != 0  || !modelIdx.isValid()  || treePtr == nullptr || treePtr->isRoot() ) {
        return;
    }

	emit deleteNode( modelIdx );

}

void binarytreeview::onAdd2NodesActionTrigger()
{
    auto colInfo = -1;
    auto selitemPr  = this->getSelectedNodeItem(&colInfo);
	auto modelIdx = selitemPr.first;
	auto treePtr = selitemPr.second;

    if ( colInfo != 0 || !modelIdx.isValid() || treePtr == nullptr  ) {
        return;
    }

	emit addBothNodes( modelIdx );
	expand( modelIdx );
}

void binarytreeview::onAddLeftNodeActionTrigger()
{
    auto colInfo = -1;
	auto selItemPr = this->getSelectedNodeItem(&colInfo);
	auto modelIdx = selItemPr.first;
	auto treePtr = selItemPr.second;

    if ( colInfo != 0 || !modelIdx.isValid() || treePtr == nullptr ) {
        return;
    }

	emit addLeftNode( modelIdx );
	expand( modelIdx );
}

void binarytreeview::onAddRightNodeActionTrigger()
{
    auto colInfo = -1;
	auto selItemPr = this->getSelectedNodeItem(&colInfo);
	auto modelIdx = selItemPr.first;
	auto treePtr = selItemPr.second;

    if ( colInfo != 0 || !modelIdx.isValid() || treePtr == nullptr ) {
        return;
    }

	emit addRightNode( modelIdx );
	expand( modelIdx );
}










void binarytreeview::initEventHandler()
{
	binarytreemodel* model = qobject_cast<binarytreemodel*>( this->model() );
	if ( model != nullptr ) {
		connect(this, SIGNAL(addLeftNode(const QModelIndex&)), model, SLOT(create_AddLeftNode(const QModelIndex&)) );
		connect(this, SIGNAL(addRightNode(const QModelIndex&)), model, SLOT(create_AddRightNode(const QModelIndex&)) );
		connect(this, SIGNAL(addBothNodes(const QModelIndex&)), model,  SLOT(create_BothNodes(const QModelIndex&)) );
		connect(this, SIGNAL(deleteNode(const QModelIndex&)), model, SLOT(delete_SelectedNode(const QModelIndex&)) );
	}
}
