#include <QDebug>

#include "TreeModel.h"

#include "rapidxml.hpp" // for xml generte use
#include "rapidxml_print.hpp" // for xml generate file content

TreeModel::TreeModel(QObject* parent /*= nullptr*/)
	: QAbstractItemModel(parent)
	, m_invisibleRoot( nullptr )
    , m_pXMLDoc( nullptr )
{
    m_invisibleRoot = new TreeNode( QString(), QString(), nullptr );
	auto onlyRoot = m_invisibleRoot->appendChild();
    onlyRoot->setName( QString("Root")  );
    onlyRoot->setValue( QString("<Not Value>")  );

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

    m_pXMLDoc = new rapidxml::xml_document<char>();

    m_XmlStringList.clear();
}


// virtual 
TreeModel::~TreeModel()
{
	if( m_invisibleRoot != nullptr ) {
        delete m_invisibleRoot;
		m_invisibleRoot = nullptr;
	}

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
QVariant	TreeModel::headerData(int section, Qt::Orientation orientation, int role /* = Qt::DisplayRole*/ ) const // Q_DECL_OVERRIDE;
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

    // append element Node (name/value)

    return true;
}
