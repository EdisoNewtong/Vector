#ifndef TREE_MODEL_H
#define TREE_MODEL_H

#include <QAbstractItemModel> 
#include <QVector>
#include <QByteArray>
// #include <QPersistentModelIndex>
// #include <QStringList>
#include <QRegExp>


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
    // Read-Only , Model Data Access Function
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

    virtual 
    Qt::ItemFlags flags(const QModelIndex & index) const Q_DECL_OVERRIDE;

    //
    // Another Category
    //
    virtual bool
    setData(const QModelIndex & index, const QVariant & afterEditVal, int role) Q_DECL_OVERRIDE;

    virtual  QVariant
    headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    //
    // In such cases, the hasChildren() function can be reimplemented to provide an inexpensive way for views to check for the presence of children
    //
    virtual bool 
    hasChildren(const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;

    //
    // Resizeable
    //
    virtual bool 
    insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE; 

    virtual bool 
    removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

protected:
    // void QAbstractItemModel::beginInsertRows(const QModelIndex &parent, int first, int last) Q_DECL_OVERRIDE;

public:
    // set support number only
    void setSupportNumberOnlyFlag(bool b);

    // for save info XML  file
    bool prepareSavedString(QString& outFileContent, QModelIndex& errorNodeMidx);
    // for load XML file into TreeView
    bool loadFileIntoTreeView(const QString& filename, QString& errorMsg);

    // Create New Node at some position or Delete a selected Node
    bool prependChildNode(const QModelIndex& parent);
    bool appendChildNode(const QModelIndex& parent);

    bool insertSiblingNodeBefore(const QModelIndex& beforeNodeIdx);
    bool insertSiblingNodeAfter(const QModelIndex& afterNodeIdx);
    
    bool deleteSelectedNode(const QModelIndex& selectedNodeIdx, int iDeleteMethod);
    bool deleteSelectedNodesChildren(const QModelIndex& selectedNodeIdx,int iDeleteMethod);

    bool createParentWithChild(const QModelIndex& selectedNodeIdx, QModelIndex& newCreatedParentIdx);

    bool swapUp(const QModelIndex& selectedNodeIdx);
    bool swapDown(const QModelIndex& selectedNodeIdx);


    signals:
        void forceSetCheckBoxByLoadedFile(int isNumberMode);

protected:

    void deleteRootNode();
    void reCreateRootNode(int needCreateRoot);
    TreeNode* loadXMLContentIntoView(rapidxml::xml_node<char>* parentXmlNode, TreeNode* parentNode, QString errorMsg, int level);
    void deleteXMLDoc();

    bool checkNameIsValid( const QModelIndex& index, int* bFlagisEmptyString) const;


    // for fill xml node
    void fillXMLHeader();
    bool fillXMLContentByNode(const QModelIndex& nodeMidx, rapidxml::xml_node<char>* parentXmlNode, QModelIndex& errorNodeMidx);

protected:
    TreeNode*  m_invisibleRoot;

    rapidxml::xml_document<char>* m_pXMLDoc;

    QVector<QByteArray> m_XmlStringList;
    QByteArray          m_loadedFileContent;

    int                 m_deleteMethod;
    int                 m_pushExistedNodeFlag;

    TreeNode*           m_existedWillInsertNode;

    QRegExp             m_nameReg;

    bool                m_isSupportNumberOnly;
    int                 m_modeFromXMLFile;

};

#endif
