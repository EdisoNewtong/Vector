#pragma once

#include <QAbstractItemModel>
#include <QVariant>
#include <QPersistentModelIndex>
#include <QVector>
#include <QByteArray>
#include <QMap>

#include "treenode.h"
#include "rapidxml.hpp" // for xml generte use

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


	//
	// File Operatation
	//
	bool loadFromFile(const QString& filename, QString* pErrorStr);
	bool saveToFile(const QString& filenameToSave, QString* pErrorStr, QPersistentModelIndex& errorIdx );

	bool isTreeOnlyHasRoot();
	void generateANewTree();


	// void updateSelectedDepthAndHeight( const QModelIndex& selectedRootNode );
	void  updateDepthAndHeight( treenode* selectedRootNode, QPointF* pMostRightBottomCenterPt);
	void  updateDepthAndHeightForRoot();
    QVector<treenode*>& getTreeNodes();

	int updateGlobalTree();
	void updateDeselectionIndex(const QModelIndex& unselected);
	void updateSelectionIndex(const QModelIndex& selected);



	bool preOrderTravelsal(treenode* node,  QVector<treenode*>& retList);
	bool inOrderTravelsal(treenode* node,   QVector<treenode*>& retList);
	bool postOrderTravelsal(treenode* node, QVector<treenode*>& retList);
protected slots:
	bool create_AddLeftNode(const QModelIndex& parent);
	bool create_AddRightNode(const QModelIndex& parent);
	bool create_BothNodes(const QModelIndex& parent);

	bool delete_SelectedNode(const QModelIndex& selectedItem);
protected:

	// recursively call
	void travelsalNodeForWriting(treenode* node, rapidxml::xml_node<char>* xmlparent, int level);
	void buildNodeFromReading(treenode* node, rapidxml::xml_node<char>* xmlparent, int level);

	int  calcTreeNodeDepthAndHeight(treenode* node, int layer);
	int  calcTreeGlobalDepthAndHeight(treenode* node, int layer);


	void innerPreOrderTravelsal(treenode* node, QVector<treenode*>& nodeList);
	void innerInOrderTravelsal(treenode* node, QVector<treenode*>& nodeList);
	void innerPostOrderTravelsal(treenode* node, QVector<treenode*>& nodeList);
protected:
	// invisible Root node
	treenode* m_pInvisibleRootItem;

	// the very beginning root which is shown in the treeView
	treenode* m_pRoot;

	// save to xml file
    rapidxml::xml_document<char>* m_pXMLDoc;

	QMap<int, QVector<treenode*> > m_renderCircleMap;
	QVector<treenode*>             m_allNodes;


    QVector<QByteArray> m_XmlStringList;
	QByteArray          m_XmlFileContent;


};