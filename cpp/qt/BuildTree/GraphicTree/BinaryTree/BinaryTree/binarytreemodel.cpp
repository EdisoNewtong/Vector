#include <QVariant>
#include "binarytreemodel.h"


/*
     ------------------
     | Invisible-Root  | 
     ------------------  
					 |   ----------------------------------------------------------------------------------------------
                     |   |                  Student Name     |    Math     |   English   |  Physics  |
                     |   ----------------------------------------------------------------------------------------------
                     |------ Grade 3                         |             |             |           |           
					           |--- Class-A                  |             |             |           |           
					                    |--- Tom             |     85      |      90     |     73    |           
					                    |--- Mary            |     73      |      95     |     87    |           
					           |--- Class-B                  |             |             |           |
					                    |--- Jack            |     90      |      70     |     95    |
					                    |--- Alice           |     83      |      76     |     45    |
					                    |--- Steven          |     75      |      98     |     82    |
					           |--- Class-C                  |             |             |           |
					                    |--- Jerry           |     77      |      88     |     99    |
					                    |--- Lisa            |     88      |      71     |    100    |
					                    |--- Nancy           |     99      |      93     |     75    |
					                    |--- Joey            |     88      |      74     |     84    |










*/



// static  Member initialize
const QString binarytreemodel::sc_ROOT_TEXT("Root");
const QString binarytreemodel::sc_LEFT_TEXT("Left");
const QString binarytreemodel::sc_RIGHT_TEXT("Right");


// QAbstractTableModel(parent)
binarytreemodel::binarytreemodel(QObject* parent /* = nullptr*/ )
    : QAbstractItemModel(parent)
{
	// invisible Root
	m_pInvisibleRootItem = new treenode( QString(), nullptr, true);
	// the very beginning  Root Node
    m_pRoot = m_pInvisibleRootItem->addLeftNode( QString("0") );
}


// virtual 
binarytreemodel::~binarytreemodel() // Q_DECL_OVERRIDE
{
	delete m_pInvisibleRootItem;
}

// virtual 
int binarytreemodel::rowCount(const QModelIndex &parent /* = QModelIndex() */) const // override;
{
	if ( !parent.isValid() ) {
		return 1;
	}

	treenode* parentNode = static_cast<treenode*>( parent.internalPointer() );
	if ( parentNode == nullptr ) {
		return 0;
	}

	auto leftNode = parentNode->leftNode();
	auto rightNode = parentNode->rightNode();
	if ( leftNode!=nullptr && rightNode!=nullptr ) {
		return 2;
	} else if ( leftNode!=nullptr || rightNode!=nullptr ) {
		return 1;
	} else {
		return 0;
	}

}



Qt::ItemFlags binarytreemodel::flags(const QModelIndex &index) const // Q_DECL_OVERRIDE
{
    if ( !index.isValid() ) {
        return QAbstractItemModel::flags(index);
    }

	auto flags = QAbstractItemModel::flags(index);
	if ( index.column() != 0 ) {
		flags |= (Qt::ItemIsEditable | Qt::ItemIsSelectable);
	}
    return flags;
}

// virtual 
int binarytreemodel::columnCount(const QModelIndex &parent /* = QModelIndex() */) const // override;
{
    Q_UNUSED(parent)

	return 3;
}



// virtual 
QVariant binarytreemodel::data(const QModelIndex &index, int role /* = Qt::DisplayRole */ ) const // override;
{
	if ( !index.isValid()  || (role != Qt::DisplayRole  && role != Qt::EditRole) ) {
		return QVariant();
	}

	// if ( role != Qt::DisplayRole ) {
	// 	return QVariant();
	// }

	treenode* rawNode = static_cast<treenode*>( index.internalPointer() );
	if ( rawNode == nullptr ) {
		return QVariant();
	}

	QVariant retVar;
	auto isRoot = rawNode->isRoot();
	int columnIdx = index.column();
	switch( columnIdx )
	{
	case 0:
		retVar = isRoot ? sc_ROOT_TEXT : ( (rawNode->parent()->leftNode() == rawNode) ? sc_LEFT_TEXT : sc_RIGHT_TEXT );
		break;
	case 1:
	case 2:
        retVar = rawNode->text();
		break;
    default:
		break;
	}

	return retVar;
	
}

// virtual
QModelIndex binarytreemodel::index(int row, int column, const QModelIndex &parent /* = QModelIndex() */ ) const // override;
{
	if ( !hasIndex(row,column, parent) ) {
		return QModelIndex();
	}


	treenode* parentNode = nullptr;
	if ( !parent.isValid() ) {
		parentNode = m_pInvisibleRootItem;
	} else {
		parentNode = static_cast<treenode*>( parent.internalPointer() );
	}

	treenode* currentNode = nullptr;
	if ( parentNode != nullptr ) {
		auto leftnode = parentNode->leftNode();
		auto rightnode = parentNode->rightNode();
		if ( leftnode!=nullptr && rightnode!=nullptr ) {
			if ( row == 0 ) {
				currentNode = leftnode;
				// return createIndex(0, column, leftnode);
			} else {
				currentNode = rightnode;
				// row == 1
				// return createIndex(1, column, rightnode);
			}
		} else if ( leftnode!=nullptr || rightnode!=nullptr ) {
			// return createIndex(0, column, leftnode!=nullptr ? leftnode : rightnode);
			currentNode = (leftnode != nullptr ? leftnode : rightnode);
		} else {
			// return QModelIndex();
			currentNode = nullptr;
		}
	} 

	if ( currentNode != nullptr ) {
		return createIndex(row, column, currentNode);
	} else {
		return QModelIndex();
	}

}

// virtual 
QModelIndex binarytreemodel::parent(const QModelIndex &index) const // override;
{
	if ( !index.isValid() ) {
		return QModelIndex();
	}

	treenode* nd = static_cast<treenode*>( index.internalPointer() );
	treenode* parentNode  = nd!=nullptr ? nd->parent() : nullptr;

	if ( parentNode == m_pInvisibleRootItem  ) {
		return QModelIndex();
	}

	auto pp = parentNode->parent();
	int iRow = 0;
	if ( pp == nullptr ) {
		return QModelIndex();
	} else {
		auto left  = pp->leftNode();
		auto right = pp->rightNode();
		if ( left!=nullptr && right!=nullptr ) {
			if ( parentNode == left ) {
				iRow = 0;
			} else {
				iRow = 1;
			}
		} else if ( left!=nullptr || right!=nullptr ) {
			iRow = 0;
		} else {
			return QModelIndex();
		}
	}

	return createIndex( iRow ,0, parentNode);

}

// virtual 
QVariant binarytreemodel::headerData(int section, Qt::Orientation orientation, int role) const // Q_DECL_OVERRIDE
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		QVariant v;
		if ( section == 0 ) {
			v = QString("Tag");
		} else if ( section == 1 ) {
			v = QString("Content");
		} else {
			v = QString("Style setting");
		}
		return v;
	}

	return QVariant();
}

bool binarytreemodel::create_AddLeftNode( const QModelIndex& parent)
{
	if ( !parent.isValid() ) {
		return false;
	}

	treenode* parentNode = static_cast<treenode*>( parent.internalPointer() );
	if ( parentNode == nullptr ) {
		return false;
	}

	auto childStatePr = parentNode->getChildState();
	auto logicError = childStatePr.first == 2 || (childStatePr.first == 1 && parentNode->leftNode()!=nullptr );
	Q_ASSERT_X( !logicError, "binarytreemodel::create_AddRightNode(...)", "Logic Error For Adding Left Node");

	beginInsertRows(parent, 0, 0);
    parentNode->addLeftNode("0");
	endInsertRows();
	return true;

}

bool binarytreemodel::create_AddRightNode(const QModelIndex& parent)
{
	if ( !parent.isValid() ) {
		return false;
	}

	treenode* parentNode = static_cast<treenode*>( parent.internalPointer() );
	if ( parentNode == nullptr ) {
		return false;
	}

	auto childStatePr = parentNode->getChildState();
	auto logicError = childStatePr.first == 2 || (childStatePr.first == 1 && parentNode->rightNode()!=nullptr );
	Q_ASSERT_X( !logicError, "binarytreemodel::createAndAddRightNode(...)", "Logic Error For Adding Left Node");

	int position = childStatePr.first == 0 ? 0 : 1;
	beginInsertRows(parent, position, position);
    parentNode->addRightNode( QString("0") );
	endInsertRows();
	return true;
}


bool binarytreemodel::delete_SelectedNode(const QModelIndex& selectedItem)
{
	if ( !selectedItem.isValid() ) {
		return false;
	}

	QModelIndex parentIdx = selectedItem.parent();
	// Can't Delete the Root Node of the tree
	if ( !parentIdx.isValid() ) {
		return false;
	}


	treenode* selectedNode = static_cast<treenode*>( selectedItem.internalPointer() );
	treenode* parentNode   = static_cast<treenode*>( parentIdx.internalPointer() );
	if ( parentNode == nullptr || selectedNode == nullptr ) {
		return false;
	}

	auto childState = parentNode->getChildState();
	auto childCnt = childState.first;
	auto toBeDeletedRowIdx = 0;
	auto isDeleteLeft = true;
	if ( selectedNode == parentNode->leftNode() ) {
		toBeDeletedRowIdx = 0;
		isDeleteLeft = true;
	} else {
		// selected right Node
		if ( childCnt == 2 ) {
			toBeDeletedRowIdx = 1;
		} else {
			toBeDeletedRowIdx = 0;
		}
		isDeleteLeft = false;
	}

	

	beginRemoveRows(parentIdx, toBeDeletedRowIdx, toBeDeletedRowIdx);
	if ( isDeleteLeft ) {
		parentNode->deleteLeftNode();
	} else {
        parentNode->deleteRightNode();
	}
	endRemoveRows();

	return true;

}


// virtual 
bool binarytreemodel::setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */) // Q_DECL_OVERRIDE;
{
    if (role != Qt::EditRole) {
        return false;
	}

	treenode* selectedNode = static_cast<treenode*>( index.internalPointer() );
    bool result = true;
	if ( selectedNode == nullptr ) {
		result = false;
	} else {
		selectedNode->setText( value.toString() );
		result = true;
	}

    if ( result ) {
		QVector<int> playedRoleVec{ Qt::DisplayRole, Qt::EditRole };
        emit dataChanged(index, index, playedRoleVec );
	}

    return result;
}




void binarytreemodel::clear()
{
// TODO :
}