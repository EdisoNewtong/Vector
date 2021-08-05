#ifndef TREE_MODEL_H
#define TREE_MODEL_H

#include <QAbstractItemModel> 
#include "TreeNode.h"

class TreeModel : public QAbstractItemModel
{
	Q_OBJECT
public:
    TreeModel(QObject* parent = nullptr);
    virtual ~TreeModel();


	//
	// implementation of abstract base class QAbstractItemModel
	//
	virtual int 
	columnCount(const QModelIndex &parent = QModelIndex()) const override;

	virtual QVariant 
	data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	virtual QModelIndex 
	index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

	virtual QModelIndex 
	parent(const QModelIndex &index) const override;

	virtual int 
	rowCount(const QModelIndex &parent = QModelIndex()) const override;
protected:
	TreeNode*  m_invisibleRoot;
	
};

#endif
