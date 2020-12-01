#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QItemSelection>

#include <QVector>
#include <QPair>
#include <QColor>


// #include "rapidxml.hpp"
// using namespace rapidxml;
namespace rapidxml
{
    template<class Ch> class xml_document;
    template<class Ch> class xml_node;
}

namespace Ui {
class MainWindow;
}

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

private:
    void createTreeModelIfNecessary();
    void deleteTreeModelIfNecessary();
    void createTreeAttrModelIfnecessary();
    void deleteTreeAttrModelIfnecessary();
    
    void clearXMLTree();
    void preSetupXMLTree();
    void clearXMLAttrTree();
    void preSetupAttrXMLTree();
    void buildXMLTree(QStandardItem* parent,rapidxml::xml_node<char>* node, int level);
    void refreshAttributeTree(rapidxml::xml_node<char>* node);
    void highLightNode(QStandardItem* node,int columnIdx); 
    void highLightAttribute(QStandardItem* node,int rowIdx, int columnIdx); 

    void hightLightNodeText(const QVector< QPair<QPair<int,int>, QColor>>& hightLightInfoVec);
    void dehighLightTextBox();
    void setXMLCurrentTextCursor(int cursorIdx);
    

    Ui::MainWindow *ui;


    rapidxml::xml_document<char>* m_pXMLDoc;
    bool m_parseOK;
	QStandardItemModel* m_pXMLTreeModel;
	QStandardItemModel* m_pXMLAttrTreeModel;
    QByteArray  m_XmlTextByteArray;
    

};

#endif // MAINWINDOW_H
