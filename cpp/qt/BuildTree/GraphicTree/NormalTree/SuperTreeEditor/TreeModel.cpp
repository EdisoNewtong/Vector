#include "TreeModel.h"


TreeModel::TreeModel(QObject* parent /*= nullptr*/)
	: QAbstractItemModel(parent)
	, m_invisibleRoot( nullptr )
{
    m_invisibleRoot = new TreeNode( QString(), nullptr );
	auto onlyRoot = m_invisibleRoot->appendChild();
	onlyRoot->setText( QString("Root1") );

	auto c1 = onlyRoot->appendChild();
	c1->setText( QString("child1") );
	auto c2 = onlyRoot->appendChild();
	c2->setText( QString("child2") );

    auto c11 = c1->appendChild();
	c11->setText( QString("child1-child1") );
    auto c12 = c1->appendChild();
    c12->setText( QString("child1-child2") );
}


// virtual 
TreeModel::~TreeModel()
{
	if( m_invisibleRoot != nullptr ) {
        delete m_invisibleRoot;
		m_invisibleRoot = nullptr;
	}
}

// virtual 
int TreeModel::columnCount(const QModelIndex &parent /* = QModelIndex()*/ ) const // override;
{
	return 2;
}

// virtual 
QVariant TreeModel::data(const QModelIndex &index, int role /* = Qt::DisplayRole*/ ) const // override;
{
	if ( !index.isValid() ) {
		return QVariant();
	}

	if ( role != Qt::DisplayRole ) {
		return QVariant();
	}

    TreeNode* item = static_cast<TreeNode*>( index.internalPointer() );
	if ( item && index.column() == 0 ) {
		return item->getText();
	}

	return QVariant();
}

// virtual 
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent /* = QModelIndex()*/ ) const // override;
{
	if ( !hasIndex(row,column, parent) ) {
		return QModelIndex();
	}

	TreeNode* parentItem = nullptr;
	if ( !parent.isValid() ) {
		parentItem = m_invisibleRoot;
	} else {
		parentItem = static_cast<TreeNode*>( parent.internalPointer() );
	}


	TreeNode* child = parentItem->getChild(row);
	if ( child != nullptr ) {
		return createIndex(row, column, child);
	} 

	return QModelIndex();
}

// virtual 
QModelIndex TreeModel::parent(const QModelIndex &index) const // override;
{
	if ( !index.isValid() ) {
		return QModelIndex();
	}

    TreeNode* childItem  = static_cast<TreeNode*>(index.internalPointer());
    TreeNode* parentItem = childItem->getParent();

	if ( parentItem == m_invisibleRoot ) {
		return QModelIndex();
	}

	return createIndex( parentItem->selfIndex() ,0, parentItem);
}

// virtual 
int TreeModel::rowCount(const QModelIndex &parent /* = QModelIndex()*/ ) const // override;
{
	TreeNode* parentItem = nullptr;
	// not the 1st node , maybe node at the 2nd position , 3rd position or ...
	if ( parent.column() > 0 ) {
		return 0;
	}

	if ( !parent.isValid() ) {
		// root node has no parent modelIndex
		parentItem = m_invisibleRoot;
	} else {
		parentItem = static_cast<TreeNode*>( parent.internalPointer() );
	}

	return parentItem != nullptr ? parentItem->childCount() : 0; 
}

