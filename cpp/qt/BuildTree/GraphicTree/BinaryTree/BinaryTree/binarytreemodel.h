#pragma once

#include <QAbstractItemModel>
#include <QVariant>

#include "treenode.h"

class binarytreemodel : public QAbstractItemModel
{
	Q_OBJECT

public:
	binarytreemodel(QObject* parent = nullptr);
    virtual ~binarytreemodel() Q_DECL_OVERRIDE;
public:
	// reimplemented member function
	
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
	
	virtual bool
    setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) Q_DECL_OVERRIDE;


	void clear();

	bool create_AddLeftNode( const QModelIndex& parent);
	bool create_AddRightNode(const QModelIndex& parent);
	bool delete_SelectedNode(const QModelIndex& selectedItem);



protected:
	// invisible Root node
	treenode* m_pInvisibleRootItem;

	// the very beginning root which is shown in the treeView
	treenode* m_pRoot;

public:
	static const QString sc_ROOT_TEXT;
	static const QString sc_LEFT_TEXT;
	static const QString sc_RIGHT_TEXT;

};