#include <QDebug>
#include <QMenu>
#include <QAction>

#include "TreeModel.h"

#include "rapidxml.hpp" // for xml generte use
#include "rapidxml_print.hpp" // for xml generate file content


static const QString G_NEW_NODE_NAME("UnnamedNode");
static const QString G_NEW_NODE_VALUE("");

TreeModel::TreeModel(QObject* parent /*= nullptr*/)
    : QAbstractItemModel(parent)
    , m_invisibleRoot( nullptr )
    , m_pXMLDoc( nullptr )
{
    reCreateRootNode();

    /*
    auto c1 = onlyRoot->appendChild();
    c1->setName( QString("child-1") );
    c1->setValue( QString("aaa")  );
    auto c2 = onlyRoot->appendChild();
    c2->setName( QString("child-2") );
    c2->setValue( QString("bbb")  );

    auto c11 = c1->appendChild();
    c11->setName( QString("c-1-1") );
    c11->setValue( QString("aaa-1")  );
    auto c12 = c1->appendChild();
    c12->setName( QString("c-1-2") );
    c12->setValue( QString("aaa-2")  );
    */

    m_pXMLDoc = new rapidxml::xml_document<char>();

    m_XmlStringList.clear();

}


// virtual 
TreeModel::~TreeModel()
{
    deleteRootNode();
    deleteXMLDoc();
}

// virtual 
int TreeModel::columnCount(const QModelIndex &parent /* = QModelIndex()*/ ) const // Q_DECL_OVERRIDE;
{
    return 2;
}

// virtual 
QVariant TreeModel::data(const QModelIndex &index, int role /* = Qt::DisplayRole*/ ) const // Q_DECL_OVERRIDE;
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    if ( role != Qt::DisplayRole && role != Qt::EditRole ) {
        return QVariant();
    }

    TreeNode* item = static_cast<TreeNode*>( index.internalPointer() );
    if ( item != nullptr ) {
        int idxCol = index.column();
        if ( idxCol == 0 ) {
            return item->getName();
        } else if ( idxCol == 1) {
            return item->getValue();
        } else {
            return QVariant();
        }
    }

    return QVariant();
}

// virtual 
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent /* = QModelIndex()*/ ) const // Q_DECL_OVERRIDE;
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
QModelIndex TreeModel::parent(const QModelIndex &index) const // Q_DECL_OVERRIDE;
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
int TreeModel::rowCount(const QModelIndex &parent /* = QModelIndex()*/ ) const // Q_DECL_OVERRIDE;
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


Qt::ItemFlags TreeModel::flags(const QModelIndex & index) const // Q_DECL_OVERRIDE;
{
    // Q_UNUSED(index);
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

// virtual
bool TreeModel::setData(const QModelIndex & index, const QVariant & afterEditVal, int role) // Q_DECL_OVERRIDE;
{
    if ( role == Qt::EditRole && index.isValid() ) {
        // qDebug() << "in setData() , value = " << value;
        TreeNode* editNode = static_cast<TreeNode*>( index.internalPointer() );
        if ( editNode != nullptr ) {
            auto idxCol = index.column();
            if ( idxCol == 0 ) {
                editNode->setName( afterEditVal.toString() );
            } else if ( idxCol == 1 ) {
                editNode->setValue( afterEditVal.toString() );
            }
        }
    }

    return true;
}

// virtual
QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role /* = Qt::DisplayRole*/ ) const // Q_DECL_OVERRIDE;
{
    if ( role != Qt::DisplayRole ) {
        return QVariant();
    }

    if ( orientation == Qt::Horizontal ) {
        if ( section == 0 ) {
            return QVariant( QString("Name") );
        } else if ( section == 1) {
            return QVariant( QString("Value") );
        }
    }

    return QVariant();

}


void TreeModel::deleteRootNode()
{
    if( m_invisibleRoot != nullptr ) {
        delete m_invisibleRoot;
        m_invisibleRoot = nullptr;
    }
}

void TreeModel::reCreateRootNode()
{
    deleteRootNode();

    m_invisibleRoot = new TreeNode( QString(), QString(), nullptr );
    auto onlyRoot = m_invisibleRoot->appendChild();
    onlyRoot->setName( QString("Root")  );
    onlyRoot->setValue( QString("")  );
}


void TreeModel::deleteXMLDoc()
{
    if ( m_pXMLDoc != nullptr ) {
        m_pXMLDoc->clear();

        delete m_pXMLDoc;
        m_pXMLDoc = nullptr;
    }
}


bool TreeModel::prepareSavedString(QString& outFileContent,QPersistentModelIndex& errorIdx)
{
    m_XmlStringList.clear();

    if ( m_pXMLDoc != nullptr ) {
        m_pXMLDoc->clear();

        rapidxml::xml_node<char>* dclNode = m_pXMLDoc->allocate_node(rapidxml::node_declaration);

        QByteArray ver("version");
        QByteArray verValue("1.0");
        QByteArray encoding("encoding");
        QByteArray encodingVal("UTF-8");
        m_XmlStringList.push_back( ver);
        m_XmlStringList.push_back( verValue);
        m_XmlStringList.push_back( encoding);
        m_XmlStringList.push_back( encodingVal);
        auto sz = m_XmlStringList.size();
        int idxLast4Ary[] = { 
            sz - 4,
            sz - 3,
            sz - 2,
            sz - 1
        };

        rapidxml::xml_attribute<char>* attr1 = m_pXMLDoc->allocate_attribute( m_XmlStringList.at(idxLast4Ary[0]).constData(), 
                                                                              m_XmlStringList.at(idxLast4Ary[1]).constData() );
        rapidxml::xml_attribute<char>* attr2 = m_pXMLDoc->allocate_attribute( m_XmlStringList.at(idxLast4Ary[2]).constData(), 
                                                                              m_XmlStringList.at(idxLast4Ary[3]).constData() );

        dclNode->append_attribute(attr1);
        dclNode->append_attribute(attr2);
        m_pXMLDoc->append_node(dclNode);
    } else {
        return false;
    }

    if ( m_invisibleRoot == nullptr ) {
        return false;
    }

    return true;
}



bool TreeModel::prependChildNode(const QModelIndex& selectedParent)
{
    if ( !selectedParent.isValid() ) {
        return false;
    }

    TreeNode* item = static_cast<TreeNode*>( selectedParent.internalPointer() );
    if ( item == nullptr ) {
        return false;
    }

    int firstIdx = 0;
    return this->insertRow(firstIdx,  selectedParent);
}


bool TreeModel::appendChildNode(const QModelIndex& selectedParent)
{
    if ( !selectedParent.isValid() ) {
        return false;
    }

    TreeNode* item = static_cast<TreeNode*>( selectedParent.internalPointer() );
    if ( item == nullptr ) {
        return false;
    }

    int lastIdx = item->childCount();
    return this->insertRow(lastIdx,  selectedParent);
}


bool TreeModel::insertSiblingNodeBefore(const QModelIndex& beforeNodeIdx)
{
    if ( !beforeNodeIdx.isValid() ) {
        return false;
    }

    TreeNode* targetItem = static_cast<TreeNode*>( beforeNodeIdx.internalPointer() );
    if ( targetItem == nullptr ) {
        return false;
    }

    int insertIdx = targetItem->selfIndex();
    return this->insertRow(insertIdx , beforeNodeIdx.parent() );
}

bool TreeModel::insertSiblingNodeAfter(const QModelIndex& afterNodeIdx)
{
    if ( !afterNodeIdx.isValid() ) {
        return false;
    }

    TreeNode* targetItem = static_cast<TreeNode*>( afterNodeIdx.internalPointer() );
    if ( targetItem == nullptr ) {
        return false;
    }

    int insertIdx = targetItem->selfIndex() + 1;
    return this->insertRow(insertIdx , afterNodeIdx.parent() );
}


bool TreeModel::isInvisibleRoot(TreeNode* node)
{
    return node == m_invisibleRoot;
}



// virtual 
bool TreeModel::insertRows(int row, int count, const QModelIndex &parent /* = QModelIndex() */ ) // Q_DECL_OVERRIDE
{
    if ( count != 1 ) {
        return QAbstractItemModel::insertRows(row,count, parent);
    }

    //
    // count == 1
    //
    QAbstractItemModel::beginInsertRows(parent, row, row + count - 1);

    //
    // Create a new treeNode that need to be inserted
    //
    bool checkValid = true;
    TreeNode* targetParentItem = static_cast<TreeNode*>( parent.internalPointer() );
    if ( targetParentItem != nullptr ) {
        int childrenSz = targetParentItem->childCount();
        if ( row > childrenSz || row < 0 ) {
            checkValid = false;
        } else {
            TreeNode* newInsertNode = nullptr;
            if ( row == 0 ) {
                // prepend at head
                newInsertNode = targetParentItem->prependChild();
            } else if ( row == childrenSz ) {
                // append at tail
                newInsertNode = targetParentItem->appendChild();
            } else {
                // In the middle N-th position
                newInsertNode = targetParentItem->insertNodeAtIndex(row);
            }

            if ( newInsertNode != nullptr ) {
                newInsertNode->setName( G_NEW_NODE_NAME );
            } else {
                checkValid = false;
            }
        }
    }

    QAbstractItemModel::endInsertRows();
    return checkValid;
}


bool TreeModel::deleteSelectedNode(const QModelIndex& selectedNodeIdx)
{
    if ( !selectedNodeIdx.isValid() ) {
        return false;
    }

    auto parentModelIdx = selectedNodeIdx.parent();
    if ( !parentModelIdx.isValid() ) {
        return false;
    }

    auto targetItemNeedToBeDelete = static_cast<TreeNode*>( selectedNodeIdx.internalPointer() );
    if ( targetItemNeedToBeDelete == nullptr ) {
        return false;
    }

    auto selfIdx = targetItemNeedToBeDelete->selfIndex();
    return this->removeRows(selfIdx,1, parentModelIdx);
}




// virtual 
bool TreeModel::removeRows(int row, int count, const QModelIndex &parent /* = QModelIndex() */) // Q_DECL_OVERRIDE;
{
    if ( count != 1 ) {
        return false;
    }

    QAbstractItemModel::beginRemoveRows(parent, row, row + count -1);

    //
    bool checkValid = true;
    TreeNode* targetParentItem = static_cast<TreeNode*>( parent.internalPointer() );
    if ( targetParentItem != nullptr ) {
        TreeNode* willDeleteNode = targetParentItem->getChild(row);
        if ( willDeleteNode != nullptr ) {
            delete willDeleteNode;
        } else {
            checkValid = false;
        }
    } else {
        checkValid = false;
    }

    QAbstractItemModel::endRemoveRows();
    return checkValid;
}

