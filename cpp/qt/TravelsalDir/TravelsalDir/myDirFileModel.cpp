#include "myDirFileModel.h"

myDirFileModel::myDirFileModel(QObject* parent /* = nullptr*/ )
    : QAbstractItemModel(parent)
{
}

// virtual 
myDirFileModel::~myDirFileModel() // Q_DECL_OVERRIDE
{

}


// virtual 
int myDirFileModel::rowCount(const QModelIndex &parent /* = QModelIndex()*/ ) const // Q_DECL_OVERRIDE;
{
    return 1;
}

// virtual 
int myDirFileModel::columnCount(const QModelIndex &parent /* = QModelIndex() */ ) const  // Q_DECL_OVERRIDE;
{
    return 1;
}

// virtual 
QVariant myDirFileModel::data(const QModelIndex &index, int role /* = Qt::DisplayRole */ ) const // Q_DECL_OVERRIDE;
{
    return QVariant();
}

// virtual 
QModelIndex myDirFileModel::index(int row, int column, const QModelIndex &parent /* = QModelIndex() */) const // Q_DECL_OVERRIDE;
{
    return QModelIndex();
}


// virtual 
QModelIndex myDirFileModel::parent(const QModelIndex &index) const // Q_DECL_OVERRIDE;
{
    return QModelIndex();
}


// virtual 
QVariant myDirFileModel::headerData(int section, Qt::Orientation orientation, int role) const // Q_DECL_OVERRIDE;
{
    return QVariant();
}


// virtual 
Qt::ItemFlags 
myDirFileModel::flags(const QModelIndex &index) const // Q_DECL_OVERRIDE;
{
    return QAbstractItemModel::flags(index);
}    





