#include <QVariant>
#include <QFile>
#include <QDebug>
#include <QMapIterator>
#include <QColor>

#include <QGraphicsSimpleTextItem>
#include <QFontMetricsF>
#include <QLineF>

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

	m_renderCircleMap.clear();
	m_allNodes.clear();

	// invisible Root
	m_pInvisibleRootItem = new treenode( QString(), nullptr, true);
	// the very beginning  Root Node
    m_pRoot = m_pInvisibleRootItem->addLeftNode( QString("0") );
	// Core Core Core : travelsal the whole tree once
	updateDepthAndHeight( m_pRoot , nullptr);
	calcTreeGlobalDepthAndHeight(m_pRoot, 0 );

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
		return m_pRoot!=nullptr ? 1 : 0;
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
	if ( !index.isValid()   ) {
		return QVariant();
	}

	int columnIdx = index.column();
    if ( columnIdx < 2  ) {
        if ( role != Qt::DisplayRole  && role != Qt::EditRole &&   role != Qt::BackgroundRole ) {
            return QVariant();
        }
	}

	treenode* rawNode = static_cast<treenode*>( index.internalPointer() );
	if ( rawNode == nullptr ) {
		return QVariant();
	}

	QVariant retVar;
	auto isRoot = rawNode->isRoot();
	switch( columnIdx )
	{
	case 0:
        if ( role == Qt::DisplayRole || role == Qt::EditRole ) {
            retVar = isRoot ? GlobalSetting::ROOT_TAG : ( (rawNode->parent()->leftNode() == rawNode) ? GlobalSetting::LEFT_TAG  :  GlobalSetting::RIGHT_TAG );
        } else if ( role == Qt::BackgroundRole ) {
            if ( rawNode->isSelected() ) {
				retVar = QBrush( QColor(204,232,255,255) );
			} else {
				retVar = QBrush( Qt::white );
			}
        }
		break;
	case 1:
        retVar = rawNode->text();
		break;
	case 2:
		{
            if ( role == Qt::DisplayRole || role == Qt::EditRole ) {
				retVar = rawNode->text();
            } else if ( role == Qt::BackgroundRole ) {
				retVar =  rawNode->getNodeStyle().m_circleBrushColor;
            } else if ( role == Qt::ForegroundRole ) {
				retVar = rawNode->getNodeStyle().m_textBrush;
			} else if ( role == Qt::FontRole ) {
                retVar = rawNode->getNodeStyle().m_textFont;
			} else if ( role == Qt::TextAlignmentRole ) {
                Qt::Alignment flags = (Qt::AlignHCenter | Qt::AlignVCenter);
                retVar = static_cast<unsigned int>(flags);
			} 
		}
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
			} else {
				currentNode = rightnode;
			}
		} else if ( leftnode!=nullptr || rightnode!=nullptr ) {
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

	updateDepthAndHeight( m_pRoot , nullptr);
	calcTreeGlobalDepthAndHeight(m_pRoot, 0 );

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

	updateDepthAndHeight( m_pRoot , nullptr);
	calcTreeGlobalDepthAndHeight(m_pRoot, 0 );

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

	updateDepthAndHeight( m_pRoot , nullptr);
	calcTreeGlobalDepthAndHeight(m_pRoot, 0 );

	return true;

}


// virtual 
bool binarytreemodel::setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */) // Q_DECL_OVERRIDE;
{
	auto iColomn = index.column();

    // qDebug() << "setData(" << iColomn << ")";
    if ( role != Qt::EditRole ) {
        // qDebug() << "setData(...) , role != Qt::EditRole    , return false";
        return false;
    }

	treenode* selectedNode = static_cast<treenode*>( index.internalPointer() );
    bool result = true;
	if ( selectedNode == nullptr ) {
		result = false;
	} else {
        // qDebug() << "setData(...) , setText(...) 1";
        selectedNode->setText( value.toString() );
		result = true;
	}

    if ( result ) {
        // qDebug() << "setData(...) , setText(...) 2";
		QVector<int> playedRoleVec{ Qt::DisplayRole, Qt::EditRole };
		// QVector<int> playedRoleVec{ Qt::DisplayRole, Qt::EditRole, Qt::FontRole, Qt::TextAlignmentRole, Qt::BackgroundRole, Qt::ForegroundRole   };

		if ( iColomn == 1 ) {
			auto d = 2.0 * GlobalSetting::circle_radius;
			auto textRenderObject = selectedNode->textObject();
			if ( textRenderObject != nullptr ) {
				auto fnt = textRenderObject->font();

				QFontMetricsF fmObj( fnt );
				auto dWidth = fmObj.horizontalAdvance( selectedNode->text() );
				auto dHeight = fmObj.height();
				auto x = (d - dWidth) / 2.0;
				auto y = (d - dHeight) / 2.0;

				textRenderObject->setPos( QPointF(x,y) );
			}
		}

        emit dataChanged(index, index, playedRoleVec );
        // emit dataChanged(index, index.siblingAtColumn(GlobalSetting::SPECIAL_COLUMN_INDEX), playedRoleVec );
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

	updateDepthAndHeight( m_pRoot , nullptr);
	calcTreeGlobalDepthAndHeight(m_pRoot, 0 );

	return true;
}

bool binarytreemodel::saveToFile(const QString& filenameToSave, QString* pErrorStr, QPersistentModelIndex& errorIdx )
{
	using namespace GlobalSetting;
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
	using namespace GlobalSetting;
	if ( node == nullptr || xmlparent == nullptr ) {
		return;
	}

	// 使用 [先根遍历]   根-左-右
	QString tag;
	auto isRootFlag = false;
	if ( node->isRoot() ) {
		isRootFlag = true;
		tag = GlobalSetting::ROOT_TAG;
	} else {
		if ( node->parent()->leftNode() == node ) {
			tag = GlobalSetting::LEFT_TAG;
		} else {
			tag = GlobalSetting::RIGHT_TAG;
		}
	}

	auto ele_node = m_pXMLDoc->allocate_node(rapidxml::node_element);

	if ( isRootFlag ) {
        rapidxml::xml_node<char>* settingNode = m_pXMLDoc->allocate_node(rapidxml::node_element);
		// set node's Tag
		m_XmlStringList.push_back( GLOBAL_SETTINGS_TAG.toUtf8() );
		settingNode->name( m_XmlStringList.last().constData() );

		////////////////////////////////////////////////////////////////////////////////////////////////////
		// set all attribute
		//
		
		// Scene's BG Color
		m_XmlStringList.push_back( SCENE_BG_TAG.toUtf8() );
		m_XmlStringList.push_back( scene_bg.color().name(QColor::HexArgb).toUtf8() );
		auto attrSceneBgColor = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrSceneBgColor );
		// circle's radius
		m_XmlStringList.push_back( RADIUS.toUtf8() );
		m_XmlStringList.push_back( QString("%1").arg(circle_radius).toUtf8() );
		auto attrR = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrR );
		// gap-1
		m_XmlStringList.push_back( LR_GAP1.toUtf8() );
		m_XmlStringList.push_back( QString("%1").arg(distance_between_leftright).toUtf8() );
		auto attrGap1 = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrGap1 );
		// gap-2
		m_XmlStringList.push_back( RL_GAP2.toUtf8() );
		m_XmlStringList.push_back( QString("%1").arg(distance_between_right__left).toUtf8() );
		auto attrGap2 = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrGap2 );
		// vertical layer gap
		m_XmlStringList.push_back( LAYER_GAP.toUtf8() );
		m_XmlStringList.push_back( QString("%1").arg(height_between_parent_and_children).toUtf8() );
		auto attrHGap = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrHGap );
		// left margin
		m_XmlStringList.push_back( LEFT_MARGIN_TAG.toUtf8() );
		m_XmlStringList.push_back( QString("%1").arg(left_margin).toUtf8() );
		auto attrLeftMargin = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrLeftMargin  );
		// right margin
		m_XmlStringList.push_back( RIGHT_MARGIN_TAG.toUtf8() );
		m_XmlStringList.push_back( QString("%1").arg(right_margin).toUtf8() );
		auto attrRightMargin = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrRightMargin );
		// top margin
		m_XmlStringList.push_back( TOP_MARGIN_TAG.toUtf8() );
		m_XmlStringList.push_back( QString("%1").arg(top_margin).toUtf8() );
		auto attrTopMargin = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrTopMargin );
		// bottom margin
		m_XmlStringList.push_back( BOTTOM_MARGIN_TAG.toUtf8() );
		m_XmlStringList.push_back( QString("%1").arg(bottom_margin).toUtf8() );
		auto attrBottomMargin = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrBottomMargin );

		  ////////////////////////////////////////////////////////////////////////////////////////////////////
		auto circleColor = circle_brush.color();
		auto circlePenColor = circle_outline.color();
		auto circlePenWidth = circle_outline.widthF();

		auto contentTextFont = text_font;
		auto contentTextColor = text_color.color();

		auto connectionLineColor = connection_line.color();
		auto connectionLineWidth = connection_line.widthF();

		// set circle's color's   tag
		m_XmlStringList.push_back( CIRCLE_COLOR_TAG.toUtf8() );
		// save circle color's value   : format "#AARRGGBB" in Qt
		m_XmlStringList.push_back( circleColor.name(QColor::HexArgb).toUtf8() );
		auto attrCircleColor = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrCircleColor  );

		// set circle's pen color tag
		m_XmlStringList.push_back( CIRCLE_PEN_COLOR_TAG.toUtf8() ); 
		// set circle's pen color value
		m_XmlStringList.push_back( circlePenColor.name(QColor::HexArgb).toUtf8() );
		auto attrCirclePenColor = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrCirclePenColor );

		// set circle's pen width tag
		m_XmlStringList.push_back( CIRCLE_PEN_WIDTH_TAG.toUtf8() );
		// set circle's pen width value
		m_XmlStringList.push_back( QString("%1").arg( circlePenWidth ).toUtf8() );
		auto attrCirclePenWidth = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrCirclePenWidth );


		// set text font tag
		m_XmlStringList.push_back( TEXT_FONT_NAME_TAG.toUtf8() );
		// set text font's name as value
		m_XmlStringList.push_back( contentTextFont.family().toUtf8() );
		auto attrFontName = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrFontName );


		// set text font pointsize 
		m_XmlStringList.push_back( TEXT_FONT_SIZE_TAG.toUtf8() );
		// set text font's name as value
		m_XmlStringList.push_back( QString("%1").arg(contentTextFont.pointSize()).toUtf8() );
		auto attrFontSize = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrFontSize );

		// set text color
		m_XmlStringList.push_back( TEXT_COLOR_TAG.toUtf8() );
		// set text color's value
		m_XmlStringList.push_back( contentTextColor.name(QColor::HexArgb).toUtf8() );
		auto attrTextColor = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrTextColor );

		// set connection line color
		m_XmlStringList.push_back( CONNECTION_LINE_COLOR_TAG.toUtf8() );
		// set text color's value
		m_XmlStringList.push_back( connectionLineColor.name(QColor::HexArgb).toUtf8() );
		auto attrConnectionLineColor = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrConnectionLineColor );

		// set connection line color
		m_XmlStringList.push_back( CONNECTION_LINE_WIDTH_TAG.toUtf8() );
		// set text color's value
		m_XmlStringList.push_back( QString("%1").arg( connectionLineWidth ).toUtf8() );
		auto attrConnectionLineWidth = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
		settingNode->append_attribute( attrConnectionLineWidth );

		ele_node->append_node(settingNode);
	}






	// set node's Tag
	QByteArray eleName;
	eleName.append( tag );
	m_XmlStringList.push_back( eleName );
	ele_node->name( m_XmlStringList.last().constData() );

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// set all attribute
	//
	// set Text' attribute name
	QByteArray baAttrText;
	baAttrText.append( GlobalSetting::TEXT_TAG );
	m_XmlStringList.push_back( baAttrText );

	// save node's text 
	QByteArray baText;
	baText.append( node->text() );
	m_XmlStringList.push_back( baText );
	auto attrText = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
	ele_node->append_attribute( attrText );


	auto renderStyle = node->getNodeStyle();
    auto circleColor = renderStyle.m_circleBrushColor.color();
    auto circlePenColor = renderStyle.m_circlePen.color();
    auto circlePenWidth = renderStyle.m_circlePen.widthF();

	auto contentTextFont = renderStyle.m_textFont;
	auto contentTextColor = renderStyle.m_textBrush.color();

	auto connectionLineColor = renderStyle.m_connectionLinePen.color();
	auto connectionLineWidth = renderStyle.m_connectionLinePen.widthF();


	// set circle's color's   tag
	m_XmlStringList.push_back( GlobalSetting::CIRCLE_COLOR_TAG.toUtf8() );
	// save circle color's value   : format "#AARRGGBB" in Qt
	m_XmlStringList.push_back( circleColor.name(QColor::HexArgb).toUtf8() );
	auto attrCircleColor = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
	ele_node->append_attribute( attrCircleColor  );

	// set circle's pen color tag
	m_XmlStringList.push_back( GlobalSetting::CIRCLE_PEN_COLOR_TAG.toUtf8() );
	// set circle's pen color value
	m_XmlStringList.push_back( circlePenColor.name(QColor::HexArgb).toUtf8() );
	auto attrCirclePenColor = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
	ele_node->append_attribute( attrCirclePenColor );

	// set circle's pen width tag
	m_XmlStringList.push_back( GlobalSetting::CIRCLE_PEN_WIDTH_TAG.toUtf8() );
	// set circle's pen width value
	m_XmlStringList.push_back( QString("%1").arg( circlePenWidth ).toUtf8() );
	auto attrCirclePenWidth = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
	ele_node->append_attribute( attrCirclePenWidth );


	// set text font tag
	m_XmlStringList.push_back( GlobalSetting::TEXT_FONT_NAME_TAG.toUtf8() );
	// set text font's name as value
	m_XmlStringList.push_back( contentTextFont.family().toUtf8() );
	auto attrFontName = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
	ele_node->append_attribute( attrFontName );


	// set text font pointsize 
	m_XmlStringList.push_back( GlobalSetting::TEXT_FONT_SIZE_TAG.toUtf8() );
	// set text font's name as value
	m_XmlStringList.push_back( QString("%1").arg(contentTextFont.pointSize()).toUtf8() );
	auto attrFontSize = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
	ele_node->append_attribute( attrFontSize );

	// set text color
	m_XmlStringList.push_back( GlobalSetting::TEXT_COLOR_TAG.toUtf8() );
	// set text color's value
	m_XmlStringList.push_back( contentTextColor.name(QColor::HexArgb).toUtf8() );
	auto attrTextColor = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
	ele_node->append_attribute( attrTextColor );

	// set connection line color
	m_XmlStringList.push_back( GlobalSetting::CONNECTION_LINE_COLOR_TAG.toUtf8() );
	// set text color's value
	m_XmlStringList.push_back( connectionLineColor.name(QColor::HexArgb).toUtf8() );
	auto attrConnectionLineColor = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
	ele_node->append_attribute( attrConnectionLineColor );

	// set connection line color
	m_XmlStringList.push_back( GlobalSetting::CONNECTION_LINE_WIDTH_TAG.toUtf8() );
	// set text color's value
	m_XmlStringList.push_back( QString("%1").arg( connectionLineWidth ).toUtf8() );
	auto attrConnectionLineWidth = m_pXMLDoc->allocate_attribute( m_XmlStringList[m_XmlStringList.size()-2].constData(), m_XmlStringList.last().constData() );
	ele_node->append_attribute( attrConnectionLineWidth );

	xmlparent->append_node( ele_node );

	travelsalNodeForWriting( node->leftNode(),   ele_node, level+1); // travelsal Left  Node
	travelsalNodeForWriting( node->rightNode(),  ele_node, level+1); // travelsal Right Node
}



void binarytreemodel::buildNodeFromReading(treenode* parentNode, rapidxml::xml_node<char>* xmlparent, int level)
{
	using namespace GlobalSetting;
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

		auto loadGlobalSettingFromFile = false;
		treenode* toBeAllocedNode = nullptr;
		QString tagName = child->name();
        if ( tagName == GlobalSetting::GLOBAL_SETTINGS_TAG ) {
			loadGlobalSettingFromFile = true;
			toBeAllocedNode = nullptr;
		} else if ( tagName == GlobalSetting::ROOT_TAG ) {
			m_pRoot = parentNode->addLeftNode("0");
			toBeAllocedNode = m_pRoot;
		} else if ( tagName == GlobalSetting::LEFT_TAG ) {
			toBeAllocedNode = parentNode->addLeftNode("0");
		} else if ( tagName == GlobalSetting::RIGHT_TAG ) {
			toBeAllocedNode = parentNode->addRightNode("0");
		}

		if ( !loadGlobalSettingFromFile ) {
			if ( toBeAllocedNode == nullptr ) {
				continue;
			}
		}

		//
		// Core Core Core :
		// You Must assign a new QBrush / QPen    rather than set the old brush's color
		/*
		    
				QColor c;
				c.setNamedColor( attrValue );

				newCfg.m_circleBrushColor.setColor( c );     // doesn't work ( the UI doesn't update )
				newCfg.m_circleBrushColor = QBrush( c );  // correct
		*/

		nodeStyleCfg newCfg; 
		if ( !loadGlobalSettingFromFile && toBeAllocedNode != nullptr ) {
			newCfg = toBeAllocedNode->getNodeStyle();
		}

		auto attrIdx = 0;
        for ( rapidxml::xml_attribute<char>* attr = child->first_attribute(); attr!=nullptr; attr = attr->next_attribute(),  ++attrIdx ) {
			QString attrName = attr->name();
			QString attrValue = attr->value();
			if ( attrName == GlobalSetting::TEXT_TAG ) {
				if ( toBeAllocedNode!=nullptr ) {
					toBeAllocedNode->setText( attrValue );
				}
			} else if ( attrName == GlobalSetting::CIRCLE_COLOR_TAG ) {
				QColor c;
				c.setNamedColor( attrValue );
				// cfg.m_circleBrushColor.setColor( c );
                if ( !loadGlobalSettingFromFile ) {
					newCfg.m_circleBrushColor = QBrush( c );
				} else {
                    circle_brush = QBrush(c);
				}
			} else if ( attrName == GlobalSetting::CIRCLE_PEN_COLOR_TAG ) {
				QColor c;
				c.setNamedColor( attrValue );
				// newCfg.m_circlePen.setColor( c );
				if ( !loadGlobalSettingFromFile ) {
					newCfg.m_circlePen = QPen( QBrush(c), newCfg.m_circlePen.widthF() );
				} else {
					circle_outline = QPen( QBrush(c), circle_outline.widthF() );
				}
			} else if ( attrName == GlobalSetting::CIRCLE_PEN_WIDTH_TAG ) {
				auto convertRet = false;
				auto dVal = attrValue.toDouble( &convertRet );
				if ( convertRet ) {
					// newCfg.m_circlePen.setWidthF(dVal);
					if ( !loadGlobalSettingFromFile ) {
						newCfg.m_circlePen = QPen( QBrush(newCfg.m_circlePen.color() ), dVal );
					} else {
						circle_outline = QPen( circle_outline.brush(), dVal );
					}
				}
			} else if ( attrName == GlobalSetting::TEXT_FONT_NAME_TAG ) {
				// newCfg.m_textFont.setFamily( attrValue );
				if ( !loadGlobalSettingFromFile ) {
					newCfg.m_textFont = QFont(attrValue, newCfg.m_textFont.pointSize() );
				} else {
					text_font = QFont(attrValue, text_font.pointSize() );
				}
			} else if ( attrName == GlobalSetting::TEXT_FONT_SIZE_TAG ) {
				auto convertRet = false;
				int pointsz = attrValue.toInt(&convertRet);
				if ( convertRet ) {
					// newCfg.m_textFont.setPointSize( pointsz );
					if ( !loadGlobalSettingFromFile ) {
						newCfg.m_textFont = QFont( newCfg.m_textFont.family(),  pointsz );
					} else {
						text_font = QFont( text_font.family(),  pointsz );
					}
				}
			} else if ( attrName == GlobalSetting::TEXT_COLOR_TAG ) {
				QColor c;
				c.setNamedColor( attrValue );
				// newCfg.m_textBrush.setColor( c );
				if ( !loadGlobalSettingFromFile ) {
					newCfg.m_textBrush = QBrush( c );
				} else {
					text_color = QBrush(c);
				}
			} else if ( attrName == GlobalSetting::CONNECTION_LINE_COLOR_TAG ) {
				QColor c;
				c.setNamedColor( attrValue );
				// newCfg.m_connectionLinePen.setColor( c );
				if ( !loadGlobalSettingFromFile ) {
					newCfg.m_connectionLinePen = QPen( QBrush(c), newCfg.m_connectionLinePen.widthF() );
				} else {
                    connection_line = QPen( QBrush(c), connection_line.widthF() );
				}
			} else if ( attrName == GlobalSetting::CONNECTION_LINE_WIDTH_TAG ) {
				auto convertRet = false;
				auto dVal = attrValue.toDouble( &convertRet );
				if ( convertRet ) {
					// newCfg.m_connectionLinePen.setWidthF(dVal);
					if ( !loadGlobalSettingFromFile ) {
						newCfg.m_connectionLinePen = QPen( QBrush(newCfg.m_connectionLinePen.color()), dVal );
					} else {
                        connection_line = QPen( connection_line.brush(), dVal);
					}
				}
			} else {
				//
				// For Global settings 
				//
				if ( loadGlobalSettingFromFile ) {
					if ( attrName == SCENE_BG_TAG ) {
						QColor c;
						c.setNamedColor( attrValue );
						scene_bg = QBrush( c );
					} else if ( attrName == RADIUS ) {
						auto convertRet = false;
						auto dVal = attrValue.toDouble(&convertRet);
						if ( convertRet ) {
							circle_radius = dVal;
						}
					} else if ( attrName == LR_GAP1 ) {
						auto convertRet = false;
						auto dVal = attrValue.toDouble(&convertRet);
						if ( convertRet ) {
							distance_between_leftright = dVal;
						}
					} else if ( attrName == RL_GAP2 ) {
						auto convertRet = false;
						auto dVal = attrValue.toDouble(&convertRet);
						if ( convertRet ) {
							distance_between_right__left = dVal;
						}
					} else if ( attrName == LAYER_GAP ) {
						auto convertRet = false;
						auto dVal = attrValue.toDouble(&convertRet);
						if ( convertRet ) {
							height_between_parent_and_children = dVal;
						}
					} else if ( attrName == LEFT_MARGIN_TAG ) {
						auto convertRet = false;
						auto dVal = attrValue.toDouble(&convertRet);
						if ( convertRet ) {
							left_margin = dVal;
						}
					} else if ( attrName == RIGHT_MARGIN_TAG ) {
						auto convertRet = false;
						auto dVal = attrValue.toDouble(&convertRet);
						if ( convertRet ) {
							right_margin = dVal;
						}
					} else if ( attrName == TOP_MARGIN_TAG ) {
						auto convertRet = false;
						auto dVal = attrValue.toDouble(&convertRet);
						if ( convertRet ) {
							top_margin = dVal;
						}
					} else if ( attrName == BOTTOM_MARGIN_TAG ) {
						auto convertRet = false;
						auto dVal = attrValue.toDouble(&convertRet);
						if ( convertRet ) {
							bottom_margin = dVal;
						}
					}
				}
			}
		}


		if ( !loadGlobalSettingFromFile && toBeAllocedNode!=nullptr ) {
			toBeAllocedNode->setNodeStyle(newCfg);
			buildNodeFromReading(toBeAllocedNode, child, level+1 );
		}
	}

}



bool binarytreemodel::isTreeOnlyHasRoot()
{
	return      m_pRoot!=nullptr &&
		       (m_pRoot->leftNode() == nullptr 
			&& m_pRoot->rightNode() == nullptr);
}


void binarytreemodel::generateANewTree()
{
	if ( m_pInvisibleRootItem != nullptr ) {
		beginResetModel();

		m_pInvisibleRootItem->deleteLeftNode();
		m_pRoot = m_pInvisibleRootItem->addLeftNode( QString("0") );

		endResetModel();

		updateDepthAndHeight( m_pRoot , nullptr);
		calcTreeGlobalDepthAndHeight(m_pRoot, 0 );

	}
}


void  binarytreemodel::updateDepthAndHeightForRoot()
{
	if ( m_pRoot != nullptr ) {
		updateDepthAndHeight(m_pRoot, nullptr);
		calcTreeGlobalDepthAndHeight(m_pRoot, 0 );
	}

}

//                                         以被UI层选中的结点为 根结点 ， 构建一颗树
void binarytreemodel::updateDepthAndHeight(treenode* selectedRootNode, QPointF* pMostRightBottomCenterPt)
{
    using namespace GlobalSetting;
	if ( selectedRootNode == nullptr ) {
		return;
	}

	m_renderCircleMap.clear();

	int wholeTreeHeight = calcTreeNodeDepthAndHeight( selectedRootNode,  0);
	// qDebug() << "wholeTreeHeight = " << wholeTreeHeight;


	/****************************************************************************************************
	  Core Core Core :

	    从树的最底层开始, 依次从下自上的逐层进行 计算，假定这是一颗[**满**]二叉树(除了树叶结点外，其他结点都有左右2个儿子结点)  
		因此计算 树中各个结点的位置 相对方便一些
		因为计算完下层后，上层的根的水平位置为是2个儿子结点的中点处


	*/
	QVector< QVector< QPair<double,double> > > measurement;
	measurement.resize( wholeTreeHeight + 1);

	QMapIterator<int, QVector<treenode*> > reverseIt( m_renderCircleMap );
	reverseIt.toBack();
	while ( reverseIt.hasPrevious() ) {
		reverseIt.previous();
		int depth = reverseIt.key();
		// qDebug() << depth << ". ";

		int nodeFullCnt = 1;
		for( int i = 0; i < depth; ++i ) {
			nodeFullCnt *= 2;
		}

		// calculate center point position
		measurement[depth].clear();
		double h = (circle_radius * 2 + height_between_parent_and_children) * depth + circle_radius;
		if ( depth == wholeTreeHeight ) {
			double x = 0.0;
			for( int i = 0; i < nodeFullCnt; ++i ) {
				x += circle_radius;
                measurement[depth].push_back( qMakePair(x, h) );
				if ( i == (nodeFullCnt-1)    &&   pMostRightBottomCenterPt != nullptr ) {
					pMostRightBottomCenterPt->setX( x );
                    pMostRightBottomCenterPt->setY( h );
				}

				// next : move delta step which distance with (a radius + a gap)
				x += circle_radius;
				double gap = (i%2 == 0) ? distance_between_leftright : distance_between_right__left;
				x += gap;
			}
		} else {
			for( int i = 0; i < nodeFullCnt; ++i ) {
				auto leftChildIdx  = i*2;
                auto rightChildIdx = leftChildIdx + 1;
				auto leftCx = measurement[depth+1][leftChildIdx].first;
				auto rightCx = measurement[depth+1][rightChildIdx].first;
				auto centerX = (leftCx + rightCx) / 2.0;
                measurement[depth].push_back( qMakePair(centerX, h) );
			}
		}


		for( auto ndIt = reverseIt.value().begin(); ndIt != reverseIt.value().end(); ++ndIt ) {
			treenode* nd = *ndIt;
			if ( nd !=nullptr ) {
				auto layerIdx = nd->layerIdx();
				auto belowCenterX = measurement[depth][layerIdx].first;
				auto belowCenterY =  measurement[depth][layerIdx].second;
				nd->setCenterPt_x( belowCenterX );
				nd->setCenterPt_y( belowCenterY );

				//
				// set connection line position
				//
				if ( depth > 0 ) {
					double upperLayerH = (circle_radius * 2 + height_between_parent_and_children) * (depth-1) + circle_radius;
					double centerX = 0.0;
					if ( layerIdx % 2 == 0 ) {
						centerX = (measurement[depth][layerIdx].first + measurement[depth][layerIdx+1].first) / 2.0;
					} else {
						centerX = (measurement[depth][layerIdx].first + measurement[depth][layerIdx-1].first) / 2.0;
					}

					QLineF below2upperLine( belowCenterX, belowCenterY, centerX, upperLayerH);
					QLineF upper2belowLine( centerX, upperLayerH, belowCenterX, belowCenterY );
					below2upperLine.setLength( circle_radius );
					upper2belowLine.setLength( circle_radius );

					nd->setConnectionLine_SelfDot(   below2upperLine.x2(),  below2upperLine.y2() );
					nd->setConnectionLine_ParentDot( upper2belowLine.x2(),  upper2belowLine.y2() );
				}
				// qDebug() << "\t" << nd->text() << ", l-idx = "<< layerIdx <<", (" << nd->x() << "," << nd->y() << ")";
			}
		}

	}
	

}




int binarytreemodel::calcTreeNodeDepthAndHeight(treenode* node, int layer)
{
	static const bool printDebugInfo = false;

	if ( node == nullptr ) {
		return -1;
	}

	// 使用 [先根遍历]   根-左-右
	node->setDepth( layer );

	if ( layer == 0 ) {
		node->setLayerIdx( 0 );
	} else {
		auto parent = node->parent();
		if ( parent != nullptr ) {
			auto bLeftTag = ( parent->leftNode() == node );
			node->setLayerIdx( parent->layerIdx() * 2 + (bLeftTag ? 0 : 1) );
		}
	}


	auto it = m_renderCircleMap.find( layer );
	if ( it == m_renderCircleMap.end() ) {
		QVector<treenode*> vec;
		vec.push_back( node );

		m_renderCircleMap.insert( layer, vec);
	} else {
		it.value().push_back( node );
	}


    int leftTreeHeight  = calcTreeNodeDepthAndHeight( node->leftNode(),  layer+1 );
	int rightTreeHeight = calcTreeNodeDepthAndHeight( node->rightNode(), layer+1 );
	int maxH = leftTreeHeight > rightTreeHeight ? leftTreeHeight : rightTreeHeight;
	maxH += 1;
	node->setHeight( maxH );

	if ( printDebugInfo ) {
		qDebug() << "node.text = " << node->text() << ", dep = " << node->depth() << ", h = " << node->height() << ", l-idx = " << node->layerIdx();
	}

	return maxH;
}


int binarytreemodel::updateGlobalTree()
{
	if ( m_pRoot != nullptr ) {
		return calcTreeGlobalDepthAndHeight(m_pRoot, 0);
	}

	return 0;
}



int  binarytreemodel::calcTreeGlobalDepthAndHeight(treenode* node, int layer)
{
	if ( node == nullptr ) {
		return -1;
	}

	// 使用 [先根遍历]   根-左-右
	node->setAbsDepth( layer );

	if ( layer == 0 ) {
		node->setAbsLayerIdx( 0 );
	} else {
		auto parent = node->parent();
		if ( parent != nullptr ) {
			auto bLeftTag = ( parent->leftNode() == node );
            node->setAbsLayerIdx( parent->absLayerIdx() * 2 + (bLeftTag ? 0 : 1) );
		}
	}

    int leftTreeHeight  = calcTreeGlobalDepthAndHeight( node->leftNode(),  layer+1 );
	int rightTreeHeight = calcTreeGlobalDepthAndHeight( node->rightNode(), layer+1 );
	int maxH = leftTreeHeight > rightTreeHeight ? leftTreeHeight : rightTreeHeight;
	maxH += 1;
	node->setAbsHeight( maxH );

	return maxH;
}



/*
	get All travelsaled nodes into a vector   
*/
QVector<treenode*>& binarytreemodel::getTreeNodes()
{
	m_allNodes.clear();
	for ( auto mpPrIt = m_renderCircleMap.begin(); mpPrIt != m_renderCircleMap.end(); ++mpPrIt )
	{
		for( auto nodeIt = mpPrIt.value().begin(); nodeIt != mpPrIt.value().end(); ++nodeIt ) {
			m_allNodes.push_back( *nodeIt );
		}
	}

	return m_allNodes;
}


void binarytreemodel::updateDeselectionIndex(const QModelIndex& unselected)
{
	QVector<int> playedRoleVec{ Qt::DisplayRole, Qt::EditRole, Qt::BackgroundRole };
    emit dataChanged(unselected, unselected, playedRoleVec );

	auto p = unselected.parent();
	while ( p.isValid() ) {
        emit dataChanged(p, p, playedRoleVec );
		p = p.parent();
	}

}

void binarytreemodel::updateSelectionIndex(const QModelIndex& selected)
{
	QVector<int> playedRoleVec{ Qt::DisplayRole, Qt::EditRole, Qt::BackgroundRole };
    emit dataChanged(selected, selected, playedRoleVec );

	auto p = selected.parent();
	while ( p.isValid() ) {
        emit dataChanged(p, p, playedRoleVec );
		p = p.parent();
	}

}


bool binarytreemodel::preOrderTravelsal(treenode* node, QVector<treenode*>& retList)
{
	if ( node == nullptr ) {
		return false;
	}

	retList.clear();
	innerPreOrderTravelsal(node, retList);
	return true;
}

bool binarytreemodel::inOrderTravelsal(treenode* node, QVector<treenode*>& retList)
{
	if ( node == nullptr ) {
		return false;
	}

	retList.clear();
	innerInOrderTravelsal(node, retList);
	return true;
}

bool binarytreemodel::postOrderTravelsal(treenode* node, QVector<treenode*>& retList)
{
	if ( node == nullptr ) {
		return false;
	}

	retList.clear();
	innerPostOrderTravelsal(node, retList);
	return true;
}

void binarytreemodel::innerPreOrderTravelsal(treenode* node, QVector<treenode*>& nodeList)
{
	// 1. Root 
	// 2. Left
	// 3. Right
	if ( node == nullptr ) {
		return;
	}

	nodeList.push_back(node); // 1. root
	innerPreOrderTravelsal( node->leftNode(), nodeList ); // 2. left
	innerPreOrderTravelsal( node->rightNode(), nodeList ); // 3. right
	
}

void binarytreemodel::innerInOrderTravelsal(treenode* node, QVector<treenode*>& nodeList)
{
	// 1. Left
	// 2. Root
	// 3. Right
	if ( node == nullptr ) {
		return;
	}
	
	innerInOrderTravelsal( node->leftNode(), nodeList ); // 1. left
	nodeList.push_back(node); // 2. root
	innerInOrderTravelsal( node->rightNode(), nodeList ); // 3. right
}

void binarytreemodel::innerPostOrderTravelsal(treenode* node, QVector<treenode*>& nodeList)
{
	// 1. Left
	// 2. Right
	// 3. Root
	if ( node == nullptr ) {
		return;
	}
	
	innerPostOrderTravelsal( node->leftNode(), nodeList );  // 1. left
	innerPostOrderTravelsal( node->rightNode(), nodeList ); // 2. right
	nodeList.push_back(node); // 3. root
}


