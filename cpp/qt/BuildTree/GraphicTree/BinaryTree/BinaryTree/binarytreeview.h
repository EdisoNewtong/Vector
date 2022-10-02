#ifndef BINARY_TREE_VIEW_H
#define BINARY_TREE_VIEW_H

#include <QTreeView>
#include <QModelIndex>
#include <QStringList>
#include <QPersistentModelIndex>
#include <QVector>
#include <QByteArray>
#include <QSet>


#include "mysettingbtndelegate.h"


class QMenu;
class QAction;
class QContextMenuEvent;
class QStandardItemModel;
class QStandardItem;
class mytreeitem;


namespace rapidxml {
    template<class T>  class xml_document;
    template<class T>  class xml_node;
}

class binarytreeview : public QTreeView
{
    Q_OBJECT

public:
    explicit binarytreeview(QWidget* parent = nullptr);
    virtual ~binarytreeview();


    void clearTree();
    void initTreeView(bool needCreateRoot);

    bool prepareSavedContent(QString* pContent,QPersistentModelIndex& errorIdx);
    bool loadTreeFromFile(const QString& filename, QString* pErrorStr);

public slots:
    void onDeleteNode();
    void onAdd2NodesActionTrigger();
    void onAddLeftNodeActionTrigger();
    void onAddRightNodeActionTrigger();

    void onTreeLeafItemEdited(QStandardItem* item);

protected:
    void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;
    mytreeitem* getSelectedNodeItem(int* pColInfo);

    QMenu*      m_pPopupMenu;

    QAction*    m_pDelteNodeAct; // delete node
    QAction*    m_pAddBothAct;   // add both left And right nodes
    QAction*    m_pAddLeftAct;   // add only left node
    QAction*    m_pAddRightAct;  // add only right node

    QStandardItemModel*    m_pTreeModel;   // model
	mysettingbtndelegate*  m_btnDelegate;  // styled delegate

    rapidxml::xml_document<char>* m_pXMLDoc;
    QVector<QByteArray> m_XmlStringList;

    QByteArray          m_XmlTextByteArray;
    QSet<int>           m_valueSet;

    bool travelsalTreeView(QStandardItem* itemNode, rapidxml::xml_node<char>* xmlparent,QPersistentModelIndex& errorIdx,int level);
    bool tryToBuildTree(QStandardItem* itemNode, rapidxml::xml_node<char>* xmlparent,QString* pErrorStr,int level);

};

#endif