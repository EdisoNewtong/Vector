#include "tableitemmodel.h"

#include <QBrush>
#include <QVariant>


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
