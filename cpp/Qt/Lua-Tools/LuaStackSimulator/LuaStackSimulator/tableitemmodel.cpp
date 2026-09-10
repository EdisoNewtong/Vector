#include "tableitemmodel.h"

#include <QBrush>
#include <QVariant>

#include "pugixml.hpp"



const QByteArray TableItemModel::sc_VERSION("version");
const QByteArray TableItemModel::sc_VERSION_VALUE("1.0");
const QByteArray TableItemModel::sc_ENCODING("encoding");
const QByteArray TableItemModel::sc_ENCODING_VALUE("UTF-8");
const QByteArray TableItemModel::sc_rootName("Stack");
const QByteArray TableItemModel::sc_nodeName("node");

const QByteArray TableItemModel::sc_attrDetail("detail");


TableItemModel::TableItemModel(QObject* parent /* = nullptr */ ) 
	: QAbstractTableModel(parent)
	, m_innernalData()
{
    // addLine();
}




// virtual 
TableItemModel::~TableItemModel()
{

}


// virtual 
int TableItemModel::rowCount(const QModelIndex &parent /* = QModelIndex() */ ) const  // Q_DECL_OVERRIDE;
{
    Q_UNUSED(parent)
	return m_innernalData.size();
}

// virtual 
int TableItemModel::columnCount(const QModelIndex &parent /* = QModelIndex() */) const // Q_DECL_OVERRIDE;
{
    Q_UNUSED(parent)
	return 3;
}

// virtual 
QVariant TableItemModel::data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const // Q_DECL_OVERRIDE
{


	if ( index.isValid() ) {
		int iRow = index.row();
		int iCol = index.column();
		if ( iRow < m_innernalData.size()  ) {
			if ( role == Qt::DisplayRole || role == Qt::EditRole ) {
				if ( iCol == 0 ) {
					return QVariant( QString( m_innernalData.at(iRow).command ) );
				} else if ( iCol == 1 ) {
					return QVariant( QString( m_innernalData.at(iRow).comment ) );
				}
			} else if ( role == Qt::ForegroundRole ) {
				return QBrush( iRow % 2 == 0 ? Qt::black : Qt::red );
			}
		}
	}

	return QVariant();

}

// virtual 
QVariant TableItemModel::headerData(int section, Qt::Orientation orientation, int role /* = Qt::DisplayRole*/ ) const // Q_DECL_OVERRIDE;
{
	// qDebug() << orientation << " = " << orientation << " , section = " << section;

    if ( role == Qt::DisplayRole ) {

		if ( orientation == Qt::Horizontal ) {
			if ( section == 0 ) {
				return QVariant( QString("Command") );
			} else if ( section == 1 ) {
				return QVariant( QString("Detail") );
			} else if ( section == 2 ) {
				return QVariant( QString("Execute") );
			} 
		} else {
			if ( section < m_innernalData.size() ) {
				return QVariant( QString("#%1").arg( section+1 ) );
			} 
		}

    }

	return QVariant();
}


// virtual 
bool TableItemModel::setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */) // Q_DECL_OVERRIDE;
{
	Q_UNUSED(role)
	
	if ( index.isValid() ) {
		int iRow = index.row();
		int iCol = index.column();
		if (  iRow < m_innernalData.size() ) {
			if ( iCol == 0 ) {
				m_innernalData[iRow].command = value.toString();
			} else if ( iCol == 1 ) {
				m_innernalData[iRow].comment = value.toString();
			}
			return true;
		}
	}

	return false;
}	


// virtual  
Qt::ItemFlags TableItemModel::flags(const QModelIndex &index) const // Q_DECL_OVERRIDE;
{
	if ( index.isValid() &&  index.column() <= 1 ) {
		return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
	}

	return  QAbstractTableModel::flags(index);
}



void TableItemModel::addLine()
{
	int preSz = m_innernalData.size();
	if ( preSz == 0 ) {
		beginInsertRows( QModelIndex(), preSz, preSz);
	} else {
        QModelIndex idx = index(preSz-1, 0 , QModelIndex() );
        QModelIndex parentIdx = idx.parent();

        beginInsertRows( parentIdx, preSz, preSz);
	}


	RawData dummyData;
	dummyData.command = "Input the stack command here ...";
	dummyData.comment = " ... ";

	m_innernalData.push_back( dummyData );

	endInsertRows();
}


void TableItemModel::addLineWithArgs(const QString& command, const QString& detail)
{
	int preSz = m_innernalData.size();
	if ( preSz == 0 ) {
		beginInsertRows( QModelIndex(), preSz, preSz);
	} else {
        QModelIndex idx = index(preSz-1, 0 , QModelIndex() );
        QModelIndex parentIdx = idx.parent();

        beginInsertRows( parentIdx, preSz, preSz);
	}


	RawData dummyData;
	dummyData.command = command;
	dummyData.comment = detail;

	m_innernalData.push_back( dummyData );

	endInsertRows();

}




bool TableItemModel::removeLines(const QVector<int>& indexes, QString& error)
{
    bool bret = true;
    if ( indexes.empty() ) {
        bret = true;
        return bret;
    }


    int datasz = m_innernalData.size();
    int sz = indexes.size();
    if ( sz == 1 ) {
        int idx = indexes.at(0);
        if ( idx>=0   &&  idx < datasz ) {
            if ( idx == (datasz-1) ) {
                QModelIndex midx = index(idx, 0 , QModelIndex() );
                QModelIndex parentIdx = midx.parent();
                beginRemoveRows(parentIdx , idx, idx);

                m_innernalData.removeLast();

                endRemoveRows();
            } else {
                error = QString("idx = %1 is not the last index").arg( idx );
                bret = false;
            }
        } else {
             error = QString("idx = %1 is out of range").arg( idx );
             bret = false;
        }
    } else {
        // sz >= 2
        int first = indexes.at(0);
        int last  = indexes.at( sz - 1 );
        if ( (first>=0 && first < datasz) && (last>=0 && last < datasz) ) {
            if ( first < last ) {
                int delta = last - first;
                if ( last == (datasz-1) ) {
                    if ( first == ((last-sz)+1) ) {
                        QModelIndex midx = index(first, 0 , QModelIndex() );
                        QModelIndex parentIdx = midx.parent();
                        beginRemoveRows( parentIdx , first, last);

                        m_innernalData.remove(first, delta+1);

                        endRemoveRows();
                    } else {
                        error = QString("first ~ last, %1 ~ %2 is not successive. ").arg(first).arg(last);
                        bret = false;
                    }
                } else {
                    error = QString("last:%1 is not the last index. ").arg( last );
                    bret = false;
                }
            } else {
                error = QString("first >= last,  %1 >= %2 ").arg( first ).arg( last );
                bret = false;
            }
        } else {
             error = QString("first:%1 or last:%2 is out of range. ").arg( first ).arg( last );
             bret = false;
        }
    }


    return bret;
}


bool TableItemModel::saveToFile(const QString& filename)
{
    using namespace pugi;

    xml_document doc;

	xml_node declaraNode = doc.append_child( pugi::node_declaration );
    xml_attribute att1 = declaraNode.append_attribute( TableItemModel::sc_VERSION.constData() );
    att1.set_value( TableItemModel::sc_VERSION_VALUE.constData() );

    xml_attribute att2 = declaraNode.append_attribute( TableItemModel::sc_ENCODING.constData() );
    att2.set_value( TableItemModel::sc_ENCODING_VALUE.constData() );

    // Create Root Node
	xml_node rootNd = doc.append_child( TableItemModel::sc_rootName.constData() );

    for( auto e : m_innernalData ) {
        xml_node nd     = rootNd.append_child( TableItemModel::sc_nodeName.constData() );
        xml_attribute commentAttr =  nd.append_attribute( TableItemModel::sc_attrDetail.constData() );
        commentAttr.set_value( e.comment.toStdString().c_str() );
        xml_node contentNd =  nd.append_child( node_pcdata );
        contentNd.set_value( e.command.toStdString().c_str()  );
    }

    return doc.save_file( filename.toStdString().c_str() );
}


bool TableItemModel::loadFromFile(const QString& filename, QString& error)
{
    using namespace pugi;
    innernalUtil utilObj(this);


    m_innernalData.clear();

    xml_document doc;
    auto parseFlag = parse_default; /* parse_full */
	xml_parse_result ret = doc.load_file(filename.toStdString().c_str(), parseFlag);
    if ( !ret ) {
        error = QString("[ERROR] %1 @offset = %2").arg( ret.description() ).arg( ret.offset );
        return false;
    }


	xml_node rootNd = doc.child( TableItemModel::sc_rootName.constData() ); 
    if ( rootNd.empty() ) {
        error = QString("[ERROR] Can't find root node : %1").arg( TableItemModel::sc_rootName.constData() );
        return false;
    }

    //  rootNd is Valid
    bool isAllAttrOK = true;
    xml_object_range<xml_node_iterator> allchildren = rootNd.children();
    for ( xml_node_iterator it  = allchildren.begin();
                            it != allchildren.end();
                          ++it )
    {
        // auto child = *it;
		// xml_attribute attribute(const char_t* name) const;
        xml_attribute commentAttr = it->attribute( TableItemModel::sc_attrDetail.constData() );
        if ( commentAttr.empty() ) {
            error = QString("[ERROR] The node has no '%1' attribute name").arg( TableItemModel::sc_attrDetail.constData() ); 
            isAllAttrOK = false;
            break;
        }

        RawData element;

        element.command = QString( it->text().get() );
        element.comment = QString( commentAttr.value() );
        m_innernalData.push_back( element );
        
    }

    if ( !isAllAttrOK ) {
        return false;
    }
    
    return true;
}




void TableItemModel::clearData()
{
    if ( m_innernalData.empty() ) {
        return;
    }

    innernalUtil utilObj(this);

    m_innernalData.clear();
}

