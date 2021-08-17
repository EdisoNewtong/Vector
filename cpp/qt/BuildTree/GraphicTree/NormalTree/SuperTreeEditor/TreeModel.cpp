
// C++ standard header
#include <string>
#include <iterator>

// Qt header
#include <QDebug>
#include <QMenu>
#include <QAction>

#include "TreeModel.h"

#include "rapidxml.hpp" // for xml generte use
#include "rapidxml_print.hpp" // for xml generate file content


static const QString G_NEW_NODE_NAME("New_Node");
static const QString G_NEW_NODE_VALUE("");

TreeModel::TreeModel(QObject* parent /*= nullptr*/)
    : QAbstractItemModel(parent)
    , m_invisibleRoot( nullptr )
    , m_pXMLDoc( nullptr )
    , m_deleteMethod(0)
    , m_pushExistedNodeFlag(0)
    , m_existedWillInsertNode( nullptr )
    , m_nameReg( QStringLiteral("[a-zA-Z_][a-zA-Z0-9_]*") )
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
    (void)parent;
    return 2;
}

// virtual 
QVariant TreeModel::data(const QModelIndex &index, int role /* = Qt::DisplayRole*/ ) const // Q_DECL_OVERRIDE;
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    QVariant val;
    TreeNode* item = static_cast<TreeNode*>( index.internalPointer() );
    if ( item != nullptr ) 
    {
        int idxCol = index.column();
        switch( role ) 
        {
            case Qt::DisplayRole:
            case Qt::EditRole:
                {
                    if ( idxCol == 0 ) {
                        val =  item->getName();
                    } else if ( idxCol == 1) {
                        val = item->getValue();
                    } 
                }
                break;
            case Qt::BackgroundRole:
            case Qt::ForegroundRole:
                {
                    if ( idxCol == 0 ) {
                        int isEmptyFlag = 0;
                        auto bValid = checkNameIsValid(index, &isEmptyFlag);
                        if ( !bValid ) {
                            if ( isEmptyFlag == 1 ) {
                                val = role == Qt::BackgroundRole ? QBrush(Qt::red) : QBrush();
                            } else if ( isEmptyFlag == 2 ) {
                                // val = role == Qt::BackgroundRole ? QBrush(Qt::red) : QBrush();
                                val = QBrush(Qt::red);
                            } else {
                                val = role == Qt::BackgroundRole ? QBrush(Qt::red) : QBrush(Qt::white);
                                // val = role == Qt::BackgroundRole ? QBrush() : QBrush(Qt::red);
                            }
                        } else {
                            val = QBrush();
                        }
                    }
                }
                break;
            default:
                break;
        }
    }

    return val;
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

    TreeNode* child = parentItem->getChildAtIdx(row);
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

    return createIndex( parentItem->selfIndex() , 0, parentItem);
}

// virtual 
int TreeModel::rowCount(const QModelIndex &parent /* = QModelIndex()*/ ) const // Q_DECL_OVERRIDE;
{
    TreeNode* parentItem = nullptr;
    // not the 1st column node , maybe node at the 2nd column position , 3rd position column or ...
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

// virtual
Qt::ItemFlags TreeModel::flags(const QModelIndex & index) const // Q_DECL_OVERRIDE;
{
    // Qt::ItemFlags retFlags = QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}



// virtual 
bool TreeModel::hasChildren(const QModelIndex &parent /* = QModelIndex() */) const // Q_DECL_OVERRIDE;
{
    TreeNode* parentItem  = nullptr;
    if ( !parent.isValid() /* || parent.column() != 0 */ ) {
        parentItem = m_invisibleRoot;
    } else {
        parentItem  = static_cast<TreeNode*>( parent.internalPointer() );
        if ( parentItem == nullptr ) {
            // qDebug() << "parentItem == nullptr, set as m_invisibleRoot";
            parentItem = m_invisibleRoot;
        }
    }

    if ( parentItem != nullptr ) {
        return parentItem->hasChildren();
    } else {
        return false;
    }
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

            // follow the QT offical document : 
            //     The dataChanged() signal should be emitted if the data was successfully set.
            emit dataChanged(index, index);
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


bool TreeModel::prepareSavedString(QString& outFileContent, QModelIndex& errorNodeMidx)
{
    m_XmlStringList.clear();

    auto bret = false;
    if ( m_pXMLDoc != nullptr && m_invisibleRoot != nullptr ) {
        m_pXMLDoc->clear();

        fillXMLHeader();
        // travelsal from RootIdx
        QModelIndex rootIdx = index(0,0);
        bret = fillXMLContentByNode(rootIdx, m_pXMLDoc, errorNodeMidx);
        if ( bret ) {
            std::string fmtStr;
            rapidxml::print( std::back_inserter(fmtStr) , *m_pXMLDoc, 0);

            // set finial data
            outFileContent = QString::fromStdString(fmtStr);
        }
    } else {
        return false;
    }

    return bret;
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


bool TreeModel::createParentWithChild(const QModelIndex& selectedNodeIdx, QModelIndex& newCreatedParentIdx)
{
    auto parentIdx = selectedNodeIdx.parent();
    TreeNode* selectedNodeParent = nullptr;
    if ( !parentIdx.isValid() ) {
        selectedNodeParent = m_invisibleRoot;
    } else {
        selectedNodeParent = static_cast<TreeNode*>( parentIdx.internalPointer() );
    }

    TreeNode* selectedNode = static_cast<TreeNode*>( selectedNodeIdx.internalPointer() );
    int selfIdx = selectedNode->selfIndex();

    bool bRet = false;
    int deleteMethod = 2;
    bRet = this->deleteSelectedNode(selectedNodeIdx, deleteMethod);
    if ( !bRet ) {
        return false;
    }
    
    bRet = this->insertRow(selfIdx, parentIdx);
    if ( !bRet ) {
        return false;
    }

    //
    // get New created pointer in selfIdx
    //
    TreeNode* newInsertedNode = selectedNodeParent->getChildAtIdx(selfIdx);
    QModelIndex newParentNodeIdx = createIndex(selfIdx, 0, newInsertedNode);
    newCreatedParentIdx = newParentNodeIdx;

    m_pushExistedNodeFlag = 1;
    m_existedWillInsertNode = selectedNode;
    bRet = this->insertRow(0, newParentNodeIdx);
    m_existedWillInsertNode = nullptr;
    m_pushExistedNodeFlag = 0;

    return bRet;
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
    if ( targetParentItem == nullptr ) {
        targetParentItem = m_invisibleRoot;
    }

    int childrenSz = targetParentItem->childCount();
    if ( row > childrenSz || row < 0 ) {
        checkValid = false;
    } else {
        TreeNode* newInsertNode = nullptr;

        if ( m_pushExistedNodeFlag == 0 ) {
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
        } else if ( m_pushExistedNodeFlag == 1 ) {
            // push Existed node
            if ( m_existedWillInsertNode ) {
                targetParentItem->pushExistedChild( m_existedWillInsertNode );
            }
        } else if ( m_pushExistedNodeFlag == 2 ) {
            // qDebug() << " Up/Down" << 
            if ( m_existedWillInsertNode ) {
                targetParentItem->insertExistedChild( m_existedWillInsertNode, row);
            }
        }
    }

    QAbstractItemModel::endInsertRows();
    return checkValid;
}


bool TreeModel::deleteSelectedNode(const QModelIndex& selectedNodeIdx, int iDeleteMethod)
{
    if ( !selectedNodeIdx.isValid() ) {
        return false;
    }

    auto parentModelIdx = selectedNodeIdx.parent();

    auto targetItemNeedToBeDelete = static_cast<TreeNode*>( selectedNodeIdx.internalPointer() );
    if ( targetItemNeedToBeDelete == nullptr ) {
        return false;
    }

    auto selfIdx = targetItemNeedToBeDelete->selfIndex();

    m_deleteMethod = iDeleteMethod;
    auto bret =  this->removeRows(selfIdx,1, parentModelIdx);
    m_deleteMethod = 0;

    return bret;
}


bool TreeModel::deleteSelectedNodesChildren(const QModelIndex& selectedNodeIdx,int iDeleteMethod)
{
    if ( !selectedNodeIdx.isValid() ) {
        return false;
    }

    auto bret = true;
    auto parentNode = static_cast<TreeNode*>( selectedNodeIdx.internalPointer() );
    for ( int i = parentNode->childCount()-1; i >=0; --i ) {
        auto childidx = this->index(i,0, selectedNodeIdx);
        if ( childidx.isValid() ) {
            bret = deleteSelectedNode(childidx, iDeleteMethod);

            if ( !bret ) {
                break;
            }
        }
        
    }

    return bret;
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
    if ( targetParentItem == nullptr ) {
        targetParentItem = m_invisibleRoot;
    }

    TreeNode* willDeleteNode = targetParentItem->getChildAtIdx(row);
    if ( willDeleteNode != nullptr ) {
        if ( m_deleteMethod == 1 ) {
            delete willDeleteNode;
        } else if ( m_deleteMethod == 2 ) {
            willDeleteNode->removeFromParent();
        } else if ( m_deleteMethod == 3 ) {
            qDebug() << "Swap Up/Down : Do removeFromParent( false )";
            willDeleteNode->removeFromParent();
        }
    } else {
        checkValid = false;
    }

    QAbstractItemModel::endRemoveRows();
    return checkValid;
}


bool TreeModel::swapUp(const QModelIndex& selectedNodeIdx)
{
    /*
     *
     * Can't Use the sample method to deal with because it will result of the value column can't be edited
     *
        auto bret = selectedNode->swapWithPreviousNode();
        ...
        emit dataChanged( newPreviousIdx,  newFormerModelIdx , roleVec);
    */

    auto parentIdx = selectedNodeIdx.parent();
    TreeNode* selectedNode = static_cast<TreeNode*>( selectedNodeIdx.internalPointer() );
    TreeNode* previousNode = selectedNode->canSwapWithPreviousNode();
    if ( previousNode == nullptr ) {
        return true;
    }

    int iRowIdx = selectedNodeIdx.row();
    auto previousRowIdx = selectedNodeIdx.sibling(iRowIdx-1, 0);

    int deleteMethod = 3;
    auto bret =  this->deleteSelectedNode(previousRowIdx , deleteMethod);
    if ( !bret ) {
        return false;
    }

    m_pushExistedNodeFlag = 2;
    m_existedWillInsertNode = previousNode;
    bret = this->insertRow(iRowIdx, parentIdx);
    m_existedWillInsertNode = nullptr;
    m_pushExistedNodeFlag = 0;
    
    return bret;
}

bool TreeModel::swapDown(const QModelIndex& selectedNodeIdx)
{
    /*
     *
     * Can't Use the sample method to deal with because it will result of the value column can't be edited
     *
        auto bret = selectedNode->swapWithPreviousNode();
        ...
        emit dataChanged( newPreviousIdx,  newFormerModelIdx , roleVec);
    */

    auto parentIdx = selectedNodeIdx.parent();
    TreeNode* selectedNode = static_cast<TreeNode*>( selectedNodeIdx.internalPointer() );
    TreeNode* formerNode   = selectedNode->canSwapWithFormerNode();
    if ( formerNode == nullptr ) {
        return true;
    }

    int iRowIdx = selectedNodeIdx.row();
    auto formerRowIdx =  selectedNodeIdx.sibling(iRowIdx + 1, 0);

    int deleteMethod = 3;
    auto bret =  this->deleteSelectedNode(formerRowIdx , deleteMethod);
    if ( !bret ) {
        return false;
    }

    m_pushExistedNodeFlag = 2;
    m_existedWillInsertNode = formerNode;
    bret = this->insertRow(iRowIdx, parentIdx);
    m_existedWillInsertNode = nullptr;
    m_pushExistedNodeFlag = 0;

    return bret;
}



bool TreeModel::checkNameIsValid(const QModelIndex& index, int* bFlagisEmptyString) const
{
    QString strData = index.data().toString();

    auto bret = m_nameReg.exactMatch(strData);
    if ( bFlagisEmptyString != nullptr ) {

        if ( strData.isEmpty() ) {
            *bFlagisEmptyString = 1;
        } else if ( strData.trimmed().isEmpty() ) {
            *bFlagisEmptyString = 2;
        } else {
            *bFlagisEmptyString = 0;
        }
        
    }
    return bret;
}


void TreeModel::fillXMLHeader()
{

    rapidxml::xml_node<char>* dclNode = m_pXMLDoc->allocate_node(rapidxml::node_declaration);

    QByteArray ver("version");
    QByteArray verValue("1.0");
    QByteArray encoding("encoding");
    QByteArray encodingVal("UTF-8");

    // For Attribute   value="..." Use
    QByteArray attrValueKey("value");

    m_XmlStringList.push_back( ver );
    m_XmlStringList.push_back( verValue );
    m_XmlStringList.push_back( encoding );
    m_XmlStringList.push_back( encodingVal );
    m_XmlStringList.push_back( encodingVal );
    m_XmlStringList.push_back( attrValueKey );

    rapidxml::xml_attribute<char>* attr1 = m_pXMLDoc->allocate_attribute( m_XmlStringList.at(0).constData(),
                                                                          m_XmlStringList.at(1).constData() );
    rapidxml::xml_attribute<char>* attr2 = m_pXMLDoc->allocate_attribute( m_XmlStringList.at(2).constData(), 
                                                                          m_XmlStringList.at(3).constData() );

    dclNode->append_attribute(attr1);
    dclNode->append_attribute(attr2);

    m_pXMLDoc->append_node(dclNode);

}


bool TreeModel::fillXMLContentByNode(const QModelIndex& nodeMidx, rapidxml::xml_node<char>* parentXmlNode, QModelIndex& errorNodeMidx)
{
    int nameIdx = 0;
    int valIdx = 0;
    auto pNode = static_cast<TreeNode*>( nodeMidx.internalPointer() );
    if ( pNode == nullptr || parentXmlNode == nullptr ) {
        return false;
    }

    if ( !checkNameIsValid( nodeMidx, nullptr ) ) {
        errorNodeMidx = nodeMidx;
        return false;
    }

    rapidxml::xml_node<char>* currentXmlNode = m_pXMLDoc->allocate_node(rapidxml::node_element);

    m_XmlStringList.push_back( pNode->getName().toUtf8() );
    nameIdx = m_XmlStringList.size() - 1;
    m_XmlStringList.push_back( pNode->getValue().toUtf8() );
    valIdx  = m_XmlStringList.size() - 1;


    currentXmlNode->name( m_XmlStringList.at(nameIdx).constData() );
    if ( !pNode->getValue().isEmpty() ) {
        if ( pNode->hasChildren() ) { 
            rapidxml::xml_attribute<char>* attrPair = m_pXMLDoc->allocate_attribute( m_XmlStringList.at(5).constData(),
                                                                                m_XmlStringList.at(valIdx).constData() );
            
            currentXmlNode->append_attribute( attrPair );
        } else {
            currentXmlNode->value( m_XmlStringList.at(valIdx).constData() );
        }
    }

    parentXmlNode->append_node( currentXmlNode );

    auto isChildAddSucc = true;
    if ( pNode->hasChildren() ) {
        int sz = pNode->childCount();
        for ( int i = 0; i < sz; ++i ) {
            QModelIndex childMidx = index(i,0, nodeMidx);
            // recursively travesal child node
            if ( !fillXMLContentByNode( childMidx ,currentXmlNode, errorNodeMidx) ) {
                isChildAddSucc = false;
                break;
            }
        }
    } 

    return isChildAddSucc;
}

