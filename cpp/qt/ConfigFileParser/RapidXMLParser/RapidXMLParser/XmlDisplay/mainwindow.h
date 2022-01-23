#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QItemSelection>

#include <QVector>
#include <QPair>
#include <QColor>
#include <QMap>


// #include "rapidxml.hpp"
// using namespace rapidxml;
namespace rapidxml
{
    template<class Ch> class xml_document;
    template<class Ch> class xml_node;
}


class xmlStandardItem;

namespace Ui {
class MainWindow;
}

class GraphicNode;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_parseBtn_clicked();
    void on_loadfileBtn_clicked();
    void on_buildTreeBtn_clicked();

    void onTreeItemSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
    void onAttributeItemSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);


    void on_useTabOrSpaceChk_clicked();

    void on_prettyFormatBtn_clicked();

    void onXmlTextBoxCursorChanged();

private:
    void createTreeModelIfNecessary();
    void deleteTreeModelIfNecessary();
    void createTreeAttrModelIfnecessary();
    void deleteTreeAttrModelIfnecessary();
    
    void clearXMLTree();
    void preSetupXMLTree();
    void clearXMLAttrTree();
    void preSetupAttrXMLTree();
    void buildXMLTree(QStandardItem* parent,GraphicNode* rootGraphicNode, rapidxml::xml_node<char>* node, int level);
    void refreshAttributeTree(rapidxml::xml_node<char>* node);
    void highLightNode(QStandardItem* node,int columnIdx); 
    void highLightAttribute(QStandardItem* node,int rowIdx, int columnIdx); 

    void hightLightNodeText(const QVector< QPair<QPair<int,int>, QColor>>& hightLightInfoVec);
    void dehighLightTextBox();
    void setXMLCurrentTextCursor(int cursorIdx);


    void refreshPostionMap(xmlStandardItem* pItem,int tp);
    void updateInheritInfo(QStandardItem* item);
    

    Ui::MainWindow *ui;


    rapidxml::xml_document<char>* m_pXMLDoc;
    bool m_parseOK;
    bool m_hasMultiRootNode;
    bool m_bIsEnableResponseCursorChanged;
    bool m_bIsEnableResponseTreeSelected;


	QStandardItemModel* m_pXMLTreeModel;
	QStandardItemModel* m_pXMLAttrTreeModel;
    QByteArray  m_XmlTextByteArray;

    QMap<int,QStandardItem*> m_xmlNodePositionMap;

    GraphicNode*  m_graphicRootNode;

};

#endif // MAINWINDOW_H
