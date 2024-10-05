#pragma once

#include <QAbstractItemModel>

class myDirFileModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	myDirFileModel(QObject* parent = nullptr);
    virtual ~myDirFileModel() Q_DECL_OVERRIDE;
protected:
    //
	// reimplemented member function
    //
	
	virtual int 
	rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	virtual int 
	columnCount(const QModelIndex &parent = QModelIndex()) const  Q_DECL_OVERRIDE;

	virtual QVariant 
	data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    virtual QModelIndex
    index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	virtual QModelIndex 
	parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

	virtual QVariant 
	headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

	virtual Qt::ItemFlags 
	flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
	

    /*
	virtual bool
    setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) Q_DECL_OVERRIDE;
    */

};

