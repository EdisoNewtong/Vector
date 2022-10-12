#include <QVariant>
#include <QFile>
#include "binarytreemodel.h"
#include "globalSettings.h"

#include "rapidxml_print.hpp" // for save tree into xml file


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





// QAbstractTableModel(parent)
binarytreemodel::binarytreemodel(QObject* parent /* = nullptr*/ )
    : QAbstractItemModel(parent)
{
	// invisible Root
	m_pInvisibleRootItem = new treenode( QString(), nullptr, true);
	// the very beginning  Root Node
    m_pRoot = m_pInvisibleRootItem->addLeftNode( QString("0") );

	m_pXMLDoc = new rapidxml::xml_document<char>();
	m_pXMLDoc->clear();
	m_XmlStringList.clear();
}


// virtual 
binarytreemodel::~binarytreemodel() // Q_DECL_OVERRIDE
{
	delete m_pInvisibleRootItem;


	if ( m_pXMLDoc != nullptr ) {
		m_pXMLDoc->clear();
		delete m_pXMLDoc;
		m_pXMLDoc = nullptr;
	}


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
        retVar = isRoot ? GlobalSetting::ROOT_TAG : ( (rawNode->parent()->leftNode() == rawNode) ? GlobalSetting::LEFT_TAG  :  GlobalSetting::RIGHT_TAG );
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


bool binarytreemodel::create_BothNodes(const QModelIndex& parent)
{
	return create_AddLeftNode(parent) && create_AddRightNode(parent);
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


bool binarytreemodel::loadFromFile(const QString& filename, QString* pErrorStr)
{
	if ( m_pInvisibleRootItem == nullptr   || m_pXMLDoc == nullptr ) {
		if ( pErrorStr != nullptr ) {
			*pErrorStr = "m_pInvisibleRootItem == nullptr || m_pRoot == nullptr || m_pXMLDoc == nullptr";
		}
		return false;
	}


	m_XmlFileContent.clear();
    QFile readFile(filename);
    if ( !readFile.open(QIODevice::ReadOnly) ) {
        if ( pErrorStr!=nullptr ) {
            *pErrorStr = QString("Can't open file %1").arg(filename);
        }
        return false;
    }
    m_XmlFileContent = readFile.readAll();

    auto parseRet = true;
	try {
		m_pXMLDoc->clear();
		m_pXMLDoc->parse<rapidxml::parse_full>( m_XmlFileContent.data() );
	} catch ( const rapidxml::parse_error& error ) {
		if ( pErrorStr != nullptr ) {
 			const auto& errLineInfo = error.getErrorLineInfo();
            *pErrorStr = QString("Parse XML Failed %1 . @%1:%2").arg( error.what() ).arg( errLineInfo.line_no ).arg( errLineInfo.col_no );
			parseRet = false;
		}
	} catch ( ... ) {
		if ( pErrorStr != nullptr ) {
            *pErrorStr = "Parse XML Failed for unexcepted reason";
			parseRet = false;
		}
	}

	if ( !parseRet ) {
		return false;
	}

	beginResetModel();
	// Delete Entire Tree
	m_pInvisibleRootItem->deleteLeftNode();
	m_pRoot = nullptr;
	// 
	int level = 0;
	buildNodeFromReading( m_pInvisibleRootItem, m_pXMLDoc, level );
	endResetModel();

	return true;
}

bool binarytreemodel::saveToFile(const QString& filenameToSave, QString* pErrorStr, QPersistentModelIndex& errorIdx )
{
	(void)errorIdx;
	if ( m_pRoot == nullptr || m_pXMLDoc == nullptr ) {
		if ( pErrorStr != nullptr ) {
            *pErrorStr = " m_pRoot == nullptr || m_pXMLDoc == nullptr ";
		}
		return false;
	}

	m_pXMLDoc->clear();
	m_XmlStringList.clear();

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

	// version="1.0"
	rapidxml::xml_attribute<char>* attr1 = m_pXMLDoc->allocate_attribute( m_XmlStringList.at(idxLast4Ary[0]).constData(), 
																		  m_XmlStringList.at(idxLast4Ary[1]).constData() );
	// encoding="UTF-8"
	rapidxml::xml_attribute<char>* attr2 = m_pXMLDoc->allocate_attribute( m_XmlStringList.at(idxLast4Ary[2]).constData(), 
																		  m_XmlStringList.at(idxLast4Ary[3]).constData() );

	dclNode->append_attribute(attr1);
	dclNode->append_attribute(attr2);
	m_pXMLDoc->append_node(dclNode);

	int begLevel = 0;
	// Core Core Core
	travelsalNodeForWriting(m_pRoot, m_pXMLDoc, begLevel);

	std::string fmtStr;
	rapidxml::print( std::back_inserter(fmtStr) , *m_pXMLDoc, 0);
	QString xmlFileContent = QString::fromStdString(fmtStr);

	// Save content into XML file
    QFile fileToSave(filenameToSave);
    if ( !fileToSave.open(QIODevice::WriteOnly) ) {
		if ( pErrorStr != nullptr ) {
            *pErrorStr = QString("open xml file : %1  Failed").arg( filenameToSave );
		}
	} 

    fileToSave.write( xmlFileContent.toUtf8() );
	return true;
}



void binarytreemodel::travelsalNodeForWriting(treenode* node, rapidxml::xml_node<char>* xmlparent, int level)
{
	if ( node == nullptr || xmlparent == nullptr ) {
		return;
	}

	// 使用 [先根遍历]   根-左-右
	QString tag;
	if ( node->isRoot() ) {
		tag = GlobalSetting::ROOT_TAG;
	} else {
		if ( node->parent()->leftNode() == node ) {
			tag = GlobalSetting::LEFT_TAG;
		} else {
			tag = GlobalSetting::RIGHT_TAG;
		}
	}

	auto ele_node = m_pXMLDoc->allocate_node(rapidxml::node_element);
	QByteArray eleName;
	// set node's Tag
	eleName.append( tag );
	m_XmlStringList.push_back( eleName );
	ele_node->name( m_XmlStringList.last().constData() );

	// set node's content
	QByteArray baText;
	baText.append( node->text() );
	m_XmlStringList.push_back( baText );

	auto attrText = m_pXMLDoc->allocate_attribute("text", m_XmlStringList.last().constData() );
	ele_node->append_attribute( attrText );
	xmlparent->append_node( ele_node );

	travelsalNodeForWriting( node->leftNode(),   ele_node, level+1); // travelsal Left  Node
	travelsalNodeForWriting( node->rightNode(),  ele_node, level+1); // travelsal Right Node
}



void binarytreemodel::buildNodeFromReading(treenode* parentNode, rapidxml::xml_node<char>* xmlparent, int level)
{
	if ( parentNode  == nullptr || xmlparent == nullptr ) {
		return;
	}

	auto idx = 0;
	for( rapidxml::xml_node<char>* child = xmlparent->first_node(); child!=nullptr; child = child->next_sibling(), ++idx )
	{
		auto tp = child->type();
		if ( tp != rapidxml::node_element ) {
			continue;
		}

		treenode* toBeAllocedNode = nullptr;
		QString tagName = child->name();
		if ( tagName == GlobalSetting::ROOT_TAG ) {
			m_pRoot = parentNode->addLeftNode("0");
			toBeAllocedNode = m_pRoot;
		} else if ( tagName == GlobalSetting::LEFT_TAG ) {
			toBeAllocedNode = parentNode->addLeftNode("0");
		} else if ( tagName == GlobalSetting::RIGHT_TAG ) {
			toBeAllocedNode = parentNode->addRightNode("0");
		}

		if ( toBeAllocedNode == nullptr ) {
			continue;
		}

		auto attrIdx = 0;
        for ( rapidxml::xml_attribute<char>* attr = child->first_attribute(); attr!=nullptr; attr = attr->next_attribute(),  ++attrIdx ) {
			QString attrName = attr->name();
			QString attrValue = attr->value();
			if ( attrName == GlobalSetting::TEXT_TAG ) {
				toBeAllocedNode->setText( attrValue );
			}
		}

		buildNodeFromReading(toBeAllocedNode, child, level+1 );
	}

}


void binarytreemodel::clear()
{
// TODO :
}