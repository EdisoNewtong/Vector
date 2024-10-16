#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "xmlStandardItem.h"
#include "GraphicNode.h"

using namespace rapidxml;

#include <QDebug>
#include <QVector>
#include <QPair>
#include <QFileDialog>
// #include <QTextCursor>
// #include <iostream>
using namespace std;

using HighlightVec = QVector< QPair<QPair<int,int>, QColor>>;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pXMLDoc(nullptr)
    , m_parseOK(false)
    , m_hasMultiRootNode(false)
    , m_bIsEnableResponseCursorChanged( true )
    , m_bIsEnableResponseTreeSelected( true )
    , m_pXMLTreeModel(nullptr)
    , m_pXMLAttrTreeModel(nullptr)
    , m_XmlTextByteArray()
    , m_xmlNodePositionMap()
    , m_graphicRootNode(nullptr)
{
    ui->setupUi(this);
    m_pXMLDoc = new xml_document<char>();

    m_xmlNodePositionMap.clear();

    connect(ui->xmlTextEdit , SIGNAL(cursorPositionChanged()), this, SLOT(onXmlTextBoxCursorChanged()) );


    createTreeModelIfNecessary();
    createTreeAttrModelIfnecessary();

    ui->xmltreeView->setStyleSheet(R"(
       QTreeView::branch:has-siblings:!adjoins-item {
            border-image: url(:/image/vline.png) 0;
        }

        QTreeView::branch:has-siblings:adjoins-item {
            border-image: url(:/image/branch-more.png) 0;
        }

        QTreeView::branch:!has-children:!has-siblings:adjoins-item {
            border-image: url(:/image/branch-end.png) 0;
        }

        QTreeView::branch:has-children:!has-siblings:closed,
        QTreeView::branch:closed:has-children:has-siblings {
                border-image: none;
                image: url(:/image/branch-closed.png);
        }

        QTreeView::branch:open:has-children:!has-siblings,
        QTreeView::branch:open:has-children:has-siblings  {
                border-image: none;
                image: url(:/image/branch-open.png);
        })"
    );


    // Qt::Unchecked || Qt::Checked || Qt::PartiallyChecked
    ui->checkBox->setCheckState(rapidxml::QtConfig<0>::supportQt4BytesFlag  ? Qt::Checked : Qt::Unchecked);
    ui->checkBox_2->setCheckState(rapidxml::QtConfig<0>::supportBlankPCDATA ? Qt::Checked : Qt::Unchecked);
    // ui->checkBox->setCheckState(Qt::PartiallyChecked);
    // ui->checkBox_2->setCheckState(Qt::Unchecked);

}

MainWindow::~MainWindow()
{
    deleteTreeModelIfNecessary();
    ui->xmltreeView->setModel(nullptr);
    deleteTreeAttrModelIfnecessary();
    ui->attributeView->setModel(nullptr);

    delete ui;

    if ( m_pXMLDoc!=nullptr ) {
        m_pXMLDoc->clear();

        delete m_pXMLDoc;
        m_pXMLDoc = nullptr;
    }

    if ( m_graphicRootNode!=nullptr ) {
        delete m_graphicRootNode;
        m_graphicRootNode = nullptr;
    }
}



void MainWindow::on_parseBtn_clicked()
{
    ui->parseResultTextBox->setPlainText("");

    m_parseOK = false;
    m_hasMultiRootNode = false;

    rapidxml::QtConfig<0>::supportQt4BytesFlag = ui->checkBox->isChecked();
    rapidxml::QtConfig<0>::supportBlankPCDATA  = ui->checkBox_2->isChecked();

    try {
        if ( m_pXMLDoc!=nullptr ) {
            m_pXMLDoc->clear();

            auto text = ui->xmlTextEdit->toPlainText();
            m_XmlTextByteArray = text.toUtf8();
            m_pXMLDoc->parse<parse_full>( m_XmlTextByteArray.data() );

            m_parseOK = true;
        }
    } catch ( const parse_error& error ) {
        ui->parseResultTextBox->setPlainText( QString("Parse XML Failed \nerror : %1\nwhere : %2").arg(error.what()).arg( error.where<char>() ) );
        const auto& errLineInfo = error.getErrorLineInfo();
        auto begIdx = errLineInfo.cursor_idx - 2;
        auto endIdx = errLineInfo.cursor_idx;
        if ( begIdx < 0 ) {
            begIdx = endIdx;
            endIdx += 2;
        } 

        //
        // High-Light the place where parse error occurs
        //
        HighlightVec hvec;
        hvec.push_back( qMakePair(qMakePair(begIdx,endIdx), Qt::red) );
        hightLightNodeText(hvec);
        setXMLCurrentTextCursor(begIdx);
    } catch ( ... ) {
        ui->parseResultTextBox->setPlainText( QString("Parse XML Failed : Unknown Error ") );
    }

    if ( m_pXMLDoc!=nullptr && m_parseOK ) {
        auto hasMultiRoot = false;

        auto elementChildCnt = 0;
        QString nodename1;
        QString nodename2;
        for ( auto child = m_pXMLDoc->first_node(); child != nullptr; child = child->next_sibling() ) {
            if ( child->type() == node_element ) {
                ++elementChildCnt;

                if ( elementChildCnt == 1 ) {
                    nodename1 = child->name();
                } else if ( elementChildCnt == 2 ) {
                    hasMultiRoot = true;
                    nodename2 = child->name();
                    break;
                }
            }
        }

        m_hasMultiRootNode = hasMultiRoot;
        if ( hasMultiRoot  ) {
            ui->parseResultTextBox->setPlainText( QStringLiteral(R"(Warning : Parse XML OK But Not !!!Well-Formed!!! Format
with multiple element node
1. %1
2. %2)").arg(nodename1).arg(nodename2) );
        } else {
            ui->parseResultTextBox->setPlainText( QStringLiteral("Parse XML Successful") );
        }



        // qDebug() << "Parse XML Successful => dehighLightTextBox()";
        dehighLightTextBox();
    } 
}

void MainWindow::on_loadfileBtn_clicked()
{
    QString pickedFileName = QFileDialog::getOpenFileName(this, QStringLiteral("Select a XML File to Read") );
    if ( pickedFileName.isNull() || pickedFileName.isEmpty() ) {
        // qDebug() << "Not select";
        return;
    }

	QFile f(pickedFileName);
	if( f.open( QIODevice::ReadOnly) ) {
		auto bytes = f.readAll();
		ui->xmlTextEdit->setPlainText(bytes);
	} else {
		ui->parseResultTextBox->setPlainText(QStringLiteral("Can't Open File   \"%1\"  \n").arg(pickedFileName));
	}
	f.close();
}

void MainWindow::on_buildTreeBtn_clicked()
{
    if ( !m_parseOK ) {
        ui->statusBar->showMessage( QStringLiteral("Parse XML Failed! Can't Build Tree") );
        return;
    }

    if ( m_hasMultiRootNode ) {
        ui->statusBar->showMessage( QStringLiteral("Multi-Root-Root, Can't Build Tree") );
        return;
    }

    // Parse Successful
    if ( m_pXMLDoc!=nullptr ) {
        clearXMLTree();
        preSetupXMLTree();
        // clearXMLAttrTree();
        // preSetupAttrXMLTree();

        auto invisibleRootItem = m_pXMLTreeModel->invisibleRootItem();
        // Create 1st XML Doc Root Node
        auto visibleRoot = new xmlStandardItem( QString("XMLDoc"), m_pXMLDoc);
        invisibleRootItem->setChild(0,0, visibleRoot);

        if( m_graphicRootNode == nullptr ) {
            m_graphicRootNode = new GraphicNode(0,visibleRoot);
        }

        m_xmlNodePositionMap.clear();
        buildXMLTree(visibleRoot, m_graphicRootNode, m_pXMLDoc, 0);
        m_graphicRootNode->updateSize();

        ui->xmltreeView->expandAll();
        ui->xmltreeView->update();
    }

}


void MainWindow::buildXMLTree(QStandardItem* parent, GraphicNode* rootGraphicNode, xml_node<char>* node, int level)
{
    if ( node == nullptr || rootGraphicNode == nullptr ) {
        return;
    }

    int idx = 0;
    for ( auto child = node->first_node(); child != nullptr; child = child->next_sibling(), ++idx ) 
    {
        auto tp = child->type();

        switch( tp )
        {
        case node_document:      //!< A document node. Name and value are empty.
            {
                // do nothing
            }
            break;
        case node_element:       //!< An element node. Name contains element name. Value contains text of first data node.
            {
                // qDebug() << "level = " << level << " , element.name = " << QString(child->name()); 
                // get children count first
                int childCount = 0;
                for ( auto subchild = child->first_node(); subchild != nullptr; subchild = subchild->next_sibling() )
                {
                    ++childCount;
                    if ( childCount >= 2 ) {
                        break;
                    }
                }

                if ( childCount == 0 ) {
                    auto xmltagItem = new xmlStandardItem( QString(child->name()), child );
                    auto xmltagValueItem = new xmlStandardItem( QString(""), child );
                    auto xmltagTypeItem = new xmlStandardItem( QString("Single-Element"), child );

                    parent->setChild(idx, 0, xmltagItem);
                    parent->setChild(idx, 1, xmltagValueItem);
                    parent->setChild(idx, 2, xmltagTypeItem);

                    refreshPostionMap(xmltagItem, 0);

                    GraphicNode* gNode = new GraphicNode(level,xmltagItem);
                    rootGraphicNode->appendChild(gNode);
                    rootGraphicNode->updateSize();
                } else if ( childCount == 1 ) {
                    auto child1st = child->first_node();
                    if ( child1st!=nullptr && child1st->type() == node_data ) {
                        auto xml1tagItem = new xmlStandardItem( QString(child->name()), child );
                        auto xmlvalueItem = new xmlStandardItem( QString(child1st->value()), child1st);
                        auto xmltypeItem = new xmlStandardItem( QString("Element-Text"), child );
                        // qDebug() << "\tidx = " << idx;

                        parent->setChild(idx, 0, xml1tagItem);
                        parent->setChild(idx, 1, xmlvalueItem);
                        parent->setChild(idx, 2, xmltypeItem );

                        refreshPostionMap(xml1tagItem, 0);
                        refreshPostionMap(xmlvalueItem, 1);

                        GraphicNode* gNode = new GraphicNode(level,xml1tagItem);
                        rootGraphicNode->appendChild(gNode);
                    } else {
                        // children count == 1  :  but 1st child is not a PCDATA node , so   recursively travelsal
                        auto newCreateParent = new xmlStandardItem( QString(child->name()), child );

                        parent->setChild(idx,0, newCreateParent);
                        parent->setChild(idx,1, nullptr);
                        parent->setChild(idx,2, nullptr);

                        refreshPostionMap(newCreateParent, 0);

                        GraphicNode* gNode = new GraphicNode(level,newCreateParent);

                        buildXMLTree(newCreateParent, gNode, child, level+1);
                        gNode->updateSize();
                    }
                } else {
                    // children count >= 2 :  recursively travelsal
                    auto newParent = new xmlStandardItem( QString(child->name()), child );
                    auto xmlvalueItem = new xmlStandardItem( QString(""), child );
                    auto xmltypeItem = new xmlStandardItem( QString("Element"), child );

                    parent->setChild(idx, 0, newParent);
                    parent->setChild(idx, 1, xmlvalueItem);
                    parent->setChild(idx, 2, xmltypeItem);

                    refreshPostionMap(newParent, 0);

                    GraphicNode* gNode = new GraphicNode(level,newParent);

                    buildXMLTree(newParent,gNode, child, level+1);
                    gNode->updateSize();
                }
            }
            break;
        case node_data:          //!< A data node. Name is empty. Value contains data text.
            {
                // node->value();
                auto valueNode = new xmlStandardItem(QString(child->value()), child );
                auto value2ndNode = new xmlStandardItem( QString(""), child );
                auto valueTypeNode = new xmlStandardItem( QString("PCDATA"), child );

                parent->setChild(idx,0, valueNode);
                parent->setChild(idx,1, value2ndNode);
                parent->setChild(idx,2, valueTypeNode);

                refreshPostionMap(valueNode, 1);

                GraphicNode* gNode = new GraphicNode(level,valueNode);
                rootGraphicNode->appendChild(gNode);
            }
            break;
        case node_cdata:         //!< A CDATA node. Name is empty. Value contains data text.
            {
                // node->value()
                auto valueNode = new xmlStandardItem( QString("<![CDATA["), child );
                auto value2ndNode = new xmlStandardItem( QString(child->value()), child );
                auto valueTypeNode = new xmlStandardItem( QString("CDATA"), child );
                parent->setChild(idx,0, valueNode);
                parent->setChild(idx,1, value2ndNode);
                parent->setChild(idx,2, valueTypeNode);

                refreshPostionMap(value2ndNode, 1);

                GraphicNode* gNode = new GraphicNode(level,valueNode);
                rootGraphicNode->appendChild(gNode);
            }
            break;
        case node_comment:       //!< A comment node. Name is empty. Value contains comment text.
            {
                // node->value()
                auto valueNode = new xmlStandardItem(QString("<!--"), child );
                auto value2ndNode = new xmlStandardItem( QString(child->value()), child );
                auto valueTypeNode = new xmlStandardItem( QString("Comment"), child );
                parent->setChild(idx,0, valueNode);
                parent->setChild(idx,1, value2ndNode);
                parent->setChild(idx,2, valueTypeNode);

                refreshPostionMap(value2ndNode, 1);

                GraphicNode* gNode = new GraphicNode(level,valueNode);
                rootGraphicNode->appendChild(gNode);
            }
            break;
        case node_declaration:   //!< A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
            {
                // node->first_attribute()
                auto valueNode = new xmlStandardItem(QString("<?xml"), child );
                auto value2ndNode = new xmlStandardItem( QString(child->value()), child );
                auto valueTypeNode = new xmlStandardItem( QString("XML Declaration"), child );
                parent->setChild(idx,0, valueNode);
                parent->setChild(idx,1, value2ndNode);
                parent->setChild(idx,2, valueTypeNode);

                refreshPostionMap(value2ndNode, 1);

                GraphicNode* gNode = new GraphicNode(level,valueNode);
                rootGraphicNode->appendChild(gNode);
            }
            break;
        case node_doctype:       //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
            {
                // node->value()
                auto valueNode = new xmlStandardItem(QString("<!DOCTYPE"), child );
                auto value2ndNode = new xmlStandardItem( QString(child->value()), child );
                auto valueTypeNode = new xmlStandardItem( QString("<!DOCTYPE"), child );
                parent->setChild(idx,0, valueNode);
                parent->setChild(idx,1, value2ndNode);
                parent->setChild(idx,2, valueTypeNode);

                refreshPostionMap(value2ndNode,  1);

                GraphicNode* gNode = new GraphicNode(level,valueNode);
                rootGraphicNode->appendChild(gNode);
            }
            break;
        case node_pi:             //!< A PI node. Name contains target. Value contains instructions.
            {
                auto valueNode = new xmlStandardItem(QString(child->name()), child );
                auto value2ndNode = new xmlStandardItem( QString(child->value()), child );
                auto valueTypeNode = new xmlStandardItem( QString("Processing Instructions"), child );
                parent->setChild(idx,0, valueNode);
                parent->setChild(idx,1, value2ndNode);
                parent->setChild(idx,2, valueTypeNode);

                refreshPostionMap(valueNode, 1);

                GraphicNode* gNode = new GraphicNode(level,valueNode);
                rootGraphicNode->appendChild(gNode);
            }
            break;
        default:
            break;
        }
    }

    rootGraphicNode->updateSize();
}


void MainWindow::refreshAttributeTree(rapidxml::xml_node<char>* node)
{
    if ( node == nullptr ) {
        return;
    } 

    static const QString noAttrStr("<No Attribute List>");

    clearXMLAttrTree();
    preSetupAttrXMLTree();
    auto attrInvisibleRootItem = m_pXMLAttrTreeModel->invisibleRootItem();

    auto tp = node->type();
    switch ( tp )
    {
    case node_document:      //!< A document node. Name and value are empty.
    case node_data:          //!< A data node. Name is empty. Value contains data text.
    case node_cdata:         //!< A CDATA node. Name is empty. Value contains data text.
    case node_comment:       //!< A comment node. Name is empty. Value contains comment text.
    case node_doctype:       //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
    case node_pi:             //!< A PI node. Name contains target. Value contains instructions.
        {
            QString headname;
            if ( tp == node_document ) {
                headname = "XMLDoc";
            } else if ( tp == node_data ) {
                headname = "PCDATA";
            } else if ( tp == node_cdata ) {
                headname = "<![CDATA[";
            } else if ( tp == node_comment ) {
                headname = "<!--";
            } else if ( tp == node_doctype ) {
                headname = "DOCTYPE";
            } else if ( tp == node_pi ) {
                headname = "Processing Instructions";
            } else {
                headname = "Unknown";
            }

            auto visibleRoot = new xmlStandardItem( QString(headname), node);
            attrInvisibleRootItem->setChild(0,0, visibleRoot);
            visibleRoot->setChild(0,0, new xmlStandardItem( noAttrStr, node) );
        }
        break;
    case node_element:       //!< An element node. Name contains element name. Value contains text of first data node.
    case node_declaration:   //!< A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
        {
            xmlStandardItem* visibleRoot = nullptr;
            
            if ( tp == node_declaration ) {
                visibleRoot = new xmlStandardItem( QString("<?xml") , node );
            } else {
                visibleRoot = new xmlStandardItem( QString( node->name() ) , node );
            }

            attrInvisibleRootItem->setChild(0,0, visibleRoot);
            int attrCnt = 0;
            for ( auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute(), ++attrCnt ) {
                auto idxItem   = new xmlStandardItem( QString( QStringLiteral("%1").arg( attrCnt+1 )  ) , node );
                auto nameItem  = new xmlStandardItem( QString(attr->name()), node );
                auto valueItem = new xmlStandardItem( QString(attr->value()), node );
                visibleRoot->setChild( attrCnt, 0, idxItem);
                visibleRoot->setChild( attrCnt, 1, nameItem);
                visibleRoot->setChild( attrCnt, 2, valueItem);
            }

            if ( attrCnt == 0 ) {
                visibleRoot->setChild(0,0, new xmlStandardItem( noAttrStr, node) );
            }
        }
        break;
    default:
        break;
    }

    ui->attributeView->expandAll();
    ui->attributeView->update();
}



void MainWindow::createTreeModelIfNecessary()
{
    if ( m_pXMLTreeModel == nullptr ) {
        m_pXMLTreeModel = new QStandardItemModel();
    }
}

void MainWindow::deleteTreeModelIfNecessary()
{
    if ( m_pXMLTreeModel != nullptr ) {
        delete m_pXMLTreeModel;
        m_pXMLTreeModel = nullptr;
    }
}

void MainWindow::createTreeAttrModelIfnecessary()
{
    if ( m_pXMLAttrTreeModel == nullptr ) {
        m_pXMLAttrTreeModel = new QStandardItemModel();
    }
}

void MainWindow::deleteTreeAttrModelIfnecessary()
{
    if ( m_pXMLAttrTreeModel != nullptr ) {
        delete m_pXMLAttrTreeModel;
        m_pXMLAttrTreeModel = nullptr;
    }
}


void MainWindow::clearXMLTree()
{
    auto selectionModel = ui->xmltreeView->selectionModel();
    if ( selectionModel!=nullptr ) {
        disconnect(selectionModel,&QItemSelectionModel::selectionChanged, this, &MainWindow::onTreeItemSelectionChanged);
        delete selectionModel;
    }

    deleteTreeModelIfNecessary();
    ui->xmltreeView->setModel(nullptr);

    if ( m_graphicRootNode!=nullptr ) {
        delete m_graphicRootNode;
        m_graphicRootNode = nullptr;
    }
}

void MainWindow::preSetupXMLTree()
{
    createTreeModelIfNecessary();

	QStringList headLabelList;
	headLabelList.push_back("Tag");
	headLabelList.push_back("Value");
	headLabelList.push_back("Type");
    m_pXMLTreeModel->setHorizontalHeaderLabels(headLabelList);

    ui->xmltreeView->setModel(m_pXMLTreeModel);

	auto selectionModel = ui->xmltreeView->selectionModel();
	if( selectionModel!=nullptr ) {
		connect(selectionModel,&QItemSelectionModel::selectionChanged, this, &MainWindow::onTreeItemSelectionChanged);
        m_bIsEnableResponseTreeSelected = true;
	}
}

void MainWindow::clearXMLAttrTree()
{
    auto selectionModel = ui->attributeView->selectionModel();
    if ( selectionModel!=nullptr ) {
        disconnect(selectionModel,&QItemSelectionModel::selectionChanged, this, &MainWindow::onAttributeItemSelectionChanged);
        delete selectionModel;
    }

    deleteTreeAttrModelIfnecessary();
    ui->attributeView->setModel(nullptr);
}

void MainWindow::preSetupAttrXMLTree()
{
    createTreeAttrModelIfnecessary();

	QStringList headLabelList;
	headLabelList.push_back("Tag/Index");
	headLabelList.push_back("name");
	headLabelList.push_back("value");
    m_pXMLAttrTreeModel->setHorizontalHeaderLabels(headLabelList);

    ui->attributeView->setModel(m_pXMLAttrTreeModel);

	auto selectionModel = ui->attributeView->selectionModel();
	if( selectionModel!=nullptr ) {
		connect(selectionModel,&QItemSelectionModel::selectionChanged, this, &MainWindow::onAttributeItemSelectionChanged);
	}
}



void MainWindow::highLightNode(QStandardItem* nodeInfo, int columnIdx)
{
    auto selectXMLInfo = dynamic_cast<xmlStandardItem*>( nodeInfo );
    if ( selectXMLInfo!=nullptr ) {
        rapidxml::xml_node<char>* xmlNode = selectXMLInfo->getXmlNode();
        if ( xmlNode!=nullptr ) {
            
            m_bIsEnableResponseCursorChanged = false;
            // disconnect(ui->xmlTextEdit , SIGNAL(cursorPositionChanged()), this, SLOT(onXmlTextBoxCursorChanged()) );

            auto tp = xmlNode->type();
            switch( tp )
            {
            case node_document:      //!< A document node. Name and value are empty.
                {
                    // do nothing
                    dehighLightTextBox();
                }
                break;
            case node_element:       //!< An element node. Name contains element name. Value contains text of first data node.
                {
                    const auto& bIsSingle = xmlNode->getIsSingleNode();
                    const auto& ots = xmlNode->getOpenTagBeginInfo();
                    const auto& ote = xmlNode->getOpenTagEndInfo();
                    const auto& cts = xmlNode->getCloseTagBeginInfo();
                    const auto& cte = xmlNode->getCloseTagEndInfo();

                    HighlightVec hvec;
                    if ( bIsSingle ) {
                        hvec.push_back( qMakePair(qMakePair(ots.cursor_idx, cte.cursor_idx), Qt::yellow) );
                    } else {
                        hvec.push_back( qMakePair(qMakePair(ots.cursor_idx,  ote.cursor_idx), Qt::yellow) );
                        hvec.push_back( qMakePair(qMakePair(ote.cursor_idx , cts.cursor_idx), Qt::red) );
                        hvec.push_back( qMakePair(qMakePair(cts.cursor_idx,  cte.cursor_idx), Qt::yellow) );
                    }

                    // qDebug() << bIsSingle << " , hvec.size() = " << hvec.size();
                    hightLightNodeText(hvec);
                    setXMLCurrentTextCursor(ots.cursor_idx);
                }
                break;
            case node_data:          //!< A data node. Name is empty. Value contains data text.
                {
                    const auto& vs = xmlNode->getValueBeginInfo();
                    const auto& ve = xmlNode->getValueEndInfo();
                    // qDebug() << "vs.cursor_idx ~ ve.cursor_idx = " << vs.cursor_idx << " , " << ve.cursor_idx;

                    HighlightVec hvec;
                    hvec.push_back( qMakePair(qMakePair(vs.cursor_idx, ve.cursor_idx), Qt::red) );
                    hightLightNodeText(hvec);
                    setXMLCurrentTextCursor( vs.cursor_idx );
                }
                break;
            case node_cdata:         //!< A CDATA node. Name is empty. Value contains data text.
            case node_comment:       //!< A comment node. Name is empty. Value contains comment text.
            case node_doctype:       //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
                {
                    const auto& ots = xmlNode->getOpenTagBeginInfo();
                    const auto& ote = xmlNode->getOpenTagEndInfo();
                    const auto& cts = xmlNode->getCloseTagBeginInfo();
                    const auto& cte = xmlNode->getCloseTagEndInfo();
                    const auto& vs = xmlNode->getValueBeginInfo();
                    const auto& ve = xmlNode->getValueEndInfo();
                    
                    HighlightVec hvec;
                    if ( columnIdx == 0 || columnIdx == 2 ) {
                        hvec.push_back( qMakePair(qMakePair(ots.cursor_idx, ote.cursor_idx), Qt::yellow) );
                        hvec.push_back( qMakePair(qMakePair(vs.cursor_idx, ve.cursor_idx), Qt::red) );
                        hvec.push_back( qMakePair(qMakePair(cts.cursor_idx, cte.cursor_idx), Qt::yellow) );
                    } else {
                        hvec.push_back( qMakePair(qMakePair(vs.cursor_idx, ve.cursor_idx), Qt::red) );
                    }

                    hightLightNodeText(hvec);
                    setXMLCurrentTextCursor( ots.cursor_idx );
                }
                break;
            case node_declaration:   //!< A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
                {
                    // e.g. <?xml version="1.0" ... ?>
                    // Open-Tag Only
                    const auto& ots = xmlNode->getOpenTagBeginInfo();
                    const auto& ote = xmlNode->getOpenTagEndInfo();
                    // qDebug() << "in case node_declaration : from " << ots.cursor_idx << " ~ " << ote.cursor_idx;
                    HighlightVec hvec;
                    hvec.push_back( qMakePair(qMakePair(ots.cursor_idx, ote.cursor_idx), Qt::yellow) );
                    hightLightNodeText(hvec);
                    setXMLCurrentTextCursor(ots.cursor_idx);

                }
                break;
            // case node_doctype:       //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
            //     {
            //         const auto& ots = xmlNode->getOpenTagBeginInfo();
            //         const auto& ote = xmlNode->getOpenTagEndInfo();
            //     }
            //     break;
            case node_pi:             //!< A PI node. Name contains target. Value contains instructions.
                {
                    const auto& ots = xmlNode->getOpenTagBeginInfo();
                    const auto& ote = xmlNode->getOpenTagEndInfo();
                    const auto& ns = xmlNode->getNameBeginInfo();
                    const auto& ne = xmlNode->getNameEndInfo();
                    const auto& vs = xmlNode->getValueBeginInfo();
                    const auto& ve = xmlNode->getValueEndInfo();

                    HighlightVec hvec;
                    if ( columnIdx == 0 ) {
                        // hight-light name()
                        hvec.push_back( qMakePair(qMakePair(ns.cursor_idx, ne.cursor_idx), Qt::yellow) );
                    } else if ( columnIdx == 1) {
                        // hight-light value()
                        hvec.push_back( qMakePair(qMakePair(vs.cursor_idx, ve.cursor_idx), Qt::red) );
                    } else {
                        hvec.push_back( qMakePair(qMakePair(ots.cursor_idx, ote.cursor_idx), Qt::yellow) );
                    }

                    hightLightNodeText(hvec);
                    setXMLCurrentTextCursor(ots.cursor_idx);
                }
                break;
            default:
                break;
            }

            // connect(ui->xmlTextEdit , SIGNAL(cursorPositionChanged()), this, SLOT(onXmlTextBoxCursorChanged()) );
            m_bIsEnableResponseCursorChanged = true;

            refreshAttributeTree(xmlNode);
        }
    }
}


void MainWindow::highLightAttribute(QStandardItem* node, int rowIdx, int columnIdx)
{
    auto xmlNode = dynamic_cast<xmlStandardItem*>(node);
    if ( xmlNode == nullptr ) {
        // qDebug() << "return 1";
        return;
    }
    auto xmlrealNode = xmlNode->getXmlNode();
    if ( xmlrealNode == nullptr ) {
        // qDebug() << "return 2";
        return;
    }


    m_bIsEnableResponseCursorChanged = false;
    // disconnect(ui->xmlTextEdit , SIGNAL(cursorPositionChanged()), this, SLOT(onXmlTextBoxCursorChanged()) );

    // xmlNode!= nullptr
    dehighLightTextBox();
    auto tp = xmlrealNode->type();
    switch ( tp ) 
    {
    case node_document:      //!< A document node. Name and value are empty.
    case node_data:          //!< A data node. Name is empty. Value contains data text.
    case node_cdata:         //!< A CDATA node. Name is empty. Value contains data text.
    case node_comment:       //!< A comment node. Name is empty. Value contains comment text.
    case node_doctype:       //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
    case node_pi:             //!< A PI node. Name contains target. Value contains instructions.
        {
            // do nothing
        }
        break;
    case node_element:       //!< An element node. Name contains element name. Value contains text of first data node.
    case node_declaration:   //!< A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
        {
            int attrCnt = 0;
            QVector< QPair<line_info,line_info> > nameVec;
            QVector< QPair<line_info,line_info> > valueVec;
            for ( auto attr = xmlrealNode->first_attribute(); attr != nullptr; attr = attr->next_attribute() ) {
                auto nameS = attr->getNameBeginInfo();
                auto nameE = attr->getNameEndInfo();
                auto valueS = attr->getValueBeginInfo();
                auto valueE = attr->getValueEndInfo();
                nameVec.push_back( qMakePair(nameS,nameE) );
                valueVec.push_back( qMakePair(valueS,valueE) );

                ++attrCnt;
            }

            // qDebug() << "in here , attrCnt = " << attrCnt << " , rowIdx = " << rowIdx << " , colIdx = " << columnIdx;
            if ( attrCnt > 0  ) {
                if ( rowIdx >=0 && rowIdx < attrCnt ) {
                    HighlightVec hvec;
                    if ( columnIdx == 1 ) {
                        // selcted name
                        auto s = nameVec.at(rowIdx).first;
                        auto e = nameVec.at(rowIdx).second;
                        hvec.push_back( qMakePair(qMakePair(s.cursor_idx,e.cursor_idx), Qt::yellow) );
                        hightLightNodeText(hvec);
                        setXMLCurrentTextCursor(s.cursor_idx);
                    } else if ( columnIdx == 2 ) {
                        // selcted value
                        auto s = valueVec.at(rowIdx).first;
                        auto e = valueVec.at(rowIdx).second;
                        hvec.push_back( qMakePair(qMakePair(s.cursor_idx,e.cursor_idx), Qt::yellow) );
                        hightLightNodeText(hvec);
                        setXMLCurrentTextCursor(s.cursor_idx);
                    } else {

                    }
                }
            } 
        }
        break;
    default:
        break;
    }

    // connect(ui->xmlTextEdit , SIGNAL(cursorPositionChanged()), this, SLOT(onXmlTextBoxCursorChanged()) );
    m_bIsEnableResponseCursorChanged = true;
    
}



void MainWindow::hightLightNodeText(const QVector< QPair<QPair<int,int>, QColor>>& hightLightInfoVec)
{
    QList<QTextEdit::ExtraSelection> lst;
    for ( const auto& info : hightLightInfoVec ) {
        auto begIdx = info.first.first;
        auto endIdx = info.first.second;
        auto highlightColor = info.second;

        auto cursor = ui->xmlTextEdit->textCursor();
        // QTextCursor currentCursor(cursor);
        
        QTextEdit::ExtraSelection selection;
        cursor.setPosition( begIdx, QTextCursor::MoveAnchor);
        cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor,  endIdx - begIdx );
        selection.cursor = cursor;
        selection.format.setBackground( QBrush(highlightColor) );

        lst.push_back(selection);
    }
    ui->xmlTextEdit->setExtraSelections(lst);
}


void MainWindow::dehighLightTextBox()
{
    QList<QTextEdit::ExtraSelection> lst;
    ui->xmlTextEdit->setExtraSelections(lst);
}


void MainWindow::setXMLCurrentTextCursor(int cursorIdx)
{
    QTextCursor currentCursor( ui->xmlTextEdit->textCursor() );
    auto realIdx = cursorIdx<=0 ? 0 : cursorIdx;
    currentCursor.setPosition(realIdx, QTextCursor::MoveAnchor);
    ui->xmlTextEdit->setTextCursor(currentCursor);
}


void MainWindow::onTreeItemSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{

    Q_UNUSED(deselected)
    if ( m_pXMLTreeModel!=nullptr &&  m_bIsEnableResponseTreeSelected ) {
        auto selectList = selected.indexes();
        if( selectList.empty() ) {
            return;
        }


        auto& selitem = selectList.at(0);
        auto selectNodeInfo = m_pXMLTreeModel->itemFromIndex( selitem  );
        if( selectNodeInfo!=nullptr ) {

	        auto selectionModel = ui->xmltreeView->selectionModel();
            if ( selectionModel != nullptr ) {
		        // disconnect(selectionModel,&QItemSelectionModel::selectionChanged, this, &MainWindow::onTreeItemSelectionChanged);
                m_bIsEnableResponseTreeSelected = false;
            }
            updateInheritInfo(selectNodeInfo);
            if ( selectionModel != nullptr ) {
		        // connect(selectionModel,&QItemSelectionModel::selectionChanged, this, &MainWindow::onTreeItemSelectionChanged);
                m_bIsEnableResponseTreeSelected = true;
            }

            highLightNode(selectNodeInfo, selitem.column() );
        }
    }
}




void MainWindow::onAttributeItemSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    Q_UNUSED(deselected)
    if ( m_pXMLAttrTreeModel!=nullptr ) {
        auto selectList = selected.indexes();
        if( selectList.empty() ) {
            return;
        }

        auto& selitem = selectList.at(0);
        auto selectNodeInfo = m_pXMLAttrTreeModel->itemFromIndex( selitem  );
        if ( selectNodeInfo!=nullptr ) {
            // qDebug() << "Do highLightAttribute";
            highLightAttribute(selectNodeInfo, selitem.row(), selitem.column() );
        }
    }
}




void MainWindow::on_useTabOrSpaceChk_clicked()
{
    auto bUseTab = ui->useTabOrSpaceChk->isChecked();
    // qDebug() << "bUseTab = " << bUseTab;
    ui->spaceNumTextBox->setEnabled(!bUseTab);
}

void MainWindow::on_prettyFormatBtn_clicked()
{
    if( m_pXMLDoc!=nullptr && m_parseOK ) {
        auto bUseTab = ui->useTabOrSpaceChk->isChecked();
        QString strSpaceCnt = ui->spaceNumTextBox->text();
        bool convertRet = false;
        auto spaceCnt = strSpaceCnt.toInt(&convertRet,10);

        if ( convertRet ) {
            rapidxml::set_use_space_instead(!bUseTab, spaceCnt );
        } else {
            // spaceCnt is invalid
        }

        std::string fmtStr;
        rapidxml::print( std::back_inserter(fmtStr) , *m_pXMLDoc, 0);
        ui->xmlTextEdit->setPlainText( QString(fmtStr.c_str()) );
    }

}



void MainWindow::refreshPostionMap(xmlStandardItem* pItem,int tp)
{
    if ( pItem == nullptr ) {
        return;
    }

    rapidxml::xml_node<char>* xmlnode = pItem->getXmlNode();
    if ( xmlnode == nullptr ) {
        return;
    }

    if ( tp == 0 ) {
        // =>   element Type
        const auto& bIsSingle = xmlnode->getIsSingleNode();
        auto ots = xmlnode->getOpenTagBeginInfo().cursor_idx;
        auto ote = xmlnode->getOpenTagEndInfo().cursor_idx;
        if ( bIsSingle ) {
            if ( ots >=0 && ote >=0 && ots < ote ) {
                for ( int i = ots; i <=ote; ++i ) {
                    auto it = m_xmlNodePositionMap.find(i);
                    if ( it == m_xmlNodePositionMap.end() ) {
                        m_xmlNodePositionMap.insert(i, pItem);
                    } else {
                        *it = pItem;
                    }
                }
            }
        } else {
            auto cts = xmlnode->getCloseTagBeginInfo().cursor_idx;
            auto cte = xmlnode->getCloseTagEndInfo().cursor_idx;
            // push open tag
            if ( ots >=0 && ote >=0 && ots < ote ) {
                for ( int i = ots; i <=ote; ++i ) {
                    auto it = m_xmlNodePositionMap.find(i);
                    if ( it == m_xmlNodePositionMap.end() ) {
                        m_xmlNodePositionMap.insert(i, pItem);
                    } else {
                        // replace
                        *it = pItem;
                    }
                }
            }

            // push end tag
            if ( cts >=0 && cte >=0 && cts < cte ) {
                for ( int i = cts; i <=cte; ++i ) {
                    auto it = m_xmlNodePositionMap.find(i);
                    if ( it == m_xmlNodePositionMap.end() ) {
                        m_xmlNodePositionMap.insert(i, pItem);
                    } else {
                        // replace
                        *it = pItem;
                    }
                }
            }
        }
    } else {
        // =>  data / value
        auto vs = xmlnode->getValueBeginInfo().cursor_idx;
        auto ve = xmlnode->getValueEndInfo().cursor_idx;
        if ( vs >=0 && ve >=0 && vs < ve ) {
            for ( int i = vs; i <=ve; ++i ) {
                auto it = m_xmlNodePositionMap.find(i);
                if ( it == m_xmlNodePositionMap.end() ) {
                    m_xmlNodePositionMap.insert(i, pItem);
                } else {
                    // replace
                    *it = pItem;
                }
            }
        }
    }
    
    
}



void MainWindow::onXmlTextBoxCursorChanged()
{
    if ( m_xmlNodePositionMap.isEmpty() ) {
        return;
    }

    if ( !m_bIsEnableResponseCursorChanged ) {
        return;
    }

    auto pos = ui->xmlTextEdit->textCursor().position();
    auto it = m_xmlNodePositionMap.find(pos);
    if ( it == m_xmlNodePositionMap.end() ) {
        return;
    }

    auto item = *it;
    if ( item!=nullptr ) {
        auto selIdx = item->index();

        auto selectionModel = ui->xmltreeView->selectionModel();
        if ( selectionModel!=nullptr ) {
            // disconnect(selectionModel,&QItemSelectionModel::selectionChanged, this, &MainWindow::onTreeItemSelectionChanged);
            m_bIsEnableResponseTreeSelected = false;
        }

        // do select
        selectionModel->select(selIdx, QItemSelectionModel::ClearAndSelect);
        ui->xmltreeView->scrollTo(selIdx);

        updateInheritInfo(item);

        if ( selectionModel!=nullptr ) {
            // connect(selectionModel,&QItemSelectionModel::selectionChanged, this, &MainWindow::onTreeItemSelectionChanged);
            m_bIsEnableResponseTreeSelected = true;
        }
    }

}


void MainWindow::updateInheritInfo(QStandardItem* item)
{
    if ( item == nullptr ) {
        return;
    }

    QVector<QString> inheritVec;
    inheritVec.clear();

    auto nRow = item->row();
    auto nCol = item->column();
    auto spFlag = false;
    
    if ( nCol == 0 ) {
        inheritVec.push_front( item->text() );
    } else {
       spFlag = true;
    }

    auto selectionModel = ui->xmltreeView->selectionModel();
    auto p = item->parent();
    while ( p != nullptr ) {
        auto needAdditionalSet = false;
        auto selP = p;
        if ( spFlag ) {
            auto leftPart = p->child(nRow,0);
            selP = leftPart; 
            if ( leftPart!=nullptr ) {
                inheritVec.push_front( leftPart->text() );
                inheritVec.push_front( p->text() );
            }
            spFlag = false;
            needAdditionalSet = true;
        } else {
            inheritVec.push_front( p->text() );
        }

        if ( selectionModel != nullptr ) {
            if ( needAdditionalSet && selP!=nullptr ) {
                selectionModel->select( selP->index(), QItemSelectionModel::Select);
            }

            selectionModel->select( p->index(), QItemSelectionModel::Select);
        }


        p = p->parent();
    }

    QString inheritMsgInfo1;
    QString inheritMsgInfo2;
    for ( int i = 0; i < inheritVec.size(); ++i ) {
        auto ele = inheritVec.at(i);
        QString indent;
        if ( i > 0 ) {
            indent = indent.fill(QChar(' '), i*2);
        }
        inheritMsgInfo1 += QString("%1%2. %3\n").arg(indent).arg(i).arg(ele);

        if ( i == inheritVec.size() - 1 ) {
            inheritMsgInfo2 += QString("%1").arg(ele);
        } else {
            inheritMsgInfo2 += QString("%1 ->").arg(ele);
        }
    }

    ui->parseResultTextBox->setPlainText( inheritMsgInfo1 );
    ui->statusBar->showMessage( inheritMsgInfo2 );
}


