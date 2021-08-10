#ifndef TREE_MODEL_H
#define TREE_MODEL_H

#include <QAbstractItemModel> 
#include <QVector>
#include <QByteArray>
// #include <QStringList>
#include <QPersistentModelIndex>


#include "TreeNode.h"

namespace rapidxml {
    template<class T>  class xml_document;
    template<class T>  class xml_node;
}


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
    columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	virtual QVariant 
    data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	virtual QModelIndex 
    index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	virtual QModelIndex 
	parent(const QModelIndex &index) const override;

	virtual int 
    rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex & index) const Q_DECL_OVERRIDE;

    virtual bool
    setData(const QModelIndex & index, const QVariant & afterEditVal, int role) Q_DECL_OVERRIDE;

    virtual  QVariant
    headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;


    // Util Operator
    bool prepareSavedString(QString& outFileContent,QPersistentModelIndex& errorIdx);
protected:

    void deleteXMLDoc();
protected:
	TreeNode*  m_invisibleRoot;

    rapidxml::xml_document<char>* m_pXMLDoc;

    QVector<QByteArray> m_XmlStringList;

};

#endif
