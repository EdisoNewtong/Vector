#include "binarytreeview.h"
#include "mytreeitem.h"

#include "rapidxml.hpp" // for xml generte use
#include "rapidxml_print.hpp" // for xml generate file content

#include <QToolTip>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include <QStandardItem>
#include <QStandardItemModel>
#include <QRect>
#include <QDebug>



// explicit
binarytreeview::binarytreeview(QWidget* parent /*= nullptr*/)
    : QTreeView(parent)
    , m_pPopupMenu(nullptr)
    , m_pDelteNodeAct(nullptr)
    , m_pAddBothAct(nullptr)
    , m_pAddLeftAct(nullptr)
    , m_pAddRightAct(nullptr)
    , m_pTreeModel(nullptr)
    , m_pXMLDoc(nullptr)
{
    m_pPopupMenu = new QMenu(this);

    m_pAddBothAct = m_pPopupMenu->addAction("Add Left && Right Nodes");
    m_pAddLeftAct = m_pPopupMenu->addAction("Add Left Node");
    m_pAddRightAct = m_pPopupMenu->addAction("Add Right Node");
    m_pPopupMenu->addSeparator();
    m_pDelteNodeAct = m_pPopupMenu->addAction("Delete select Node");

    connect( m_pAddBothAct,   SIGNAL(triggered(bool)), this, SLOT(onAdd2NodesActionTrigger()) );
    connect( m_pAddLeftAct,   SIGNAL(triggered(bool)), this, SLOT(onAddLeftNodeActionTrigger()) );
    connect( m_pAddRightAct,  SIGNAL(triggered(bool)), this, SLOT(onAddRightNodeActionTrigger()) );
    connect( m_pDelteNodeAct, SIGNAL(triggered(bool)), this, SLOT(onDeleteNode()) );

    m_pTreeModel = new QStandardItemModel();
    this->setModel(m_pTreeModel);
    initTreeView(true);
    // QStandardItemModel::itemChanged(QStandardItem *item)
    connect(m_pTreeModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onTreeLeafItemEdited(QStandardItem*) ) );

    m_pXMLDoc = new rapidxml::xml_document<char>();

    m_XmlStringList.clear();
    m_valueSet.clear();
}


// virtual 
binarytreeview::~binarytreeview()
{
    auto selectionModel = this->selectionModel();
    this->setModel(nullptr);
    if ( selectionModel!=nullptr ) {
        delete selectionModel;
        selectionModel = nullptr;
    }
    
    if ( m_pTreeModel  != nullptr ) {
        m_pTreeModel->clear();

        delete m_pTreeModel;
        m_pTreeModel = nullptr;
    }

    if ( m_pXMLDoc!=nullptr ) {
        m_pXMLDoc->clear();

        delete m_pXMLDoc;
        m_pXMLDoc = nullptr;
    }
}


mytreeitem* binarytreeview::getSelectedNodeItem(int* pColInfo)
{
    auto selLst = this->selectedIndexes();
    mytreeitem* selitem = nullptr;
    if ( selLst.size() == 1 ) {
        auto selidx = selLst.at(0);
        auto nCol = selidx.column();
        if ( pColInfo!=nullptr ) {
            *pColInfo = nCol;
        }

        if ( nCol!=0 ) {
            return nullptr;
        }
        
        QStandardItemModel* smodel = dynamic_cast<QStandardItemModel*>(this->model());
        if ( smodel!=nullptr ) {
            selitem = dynamic_cast<mytreeitem*>(smodel->itemFromIndex(selidx));
        }
    }

    return selitem;
}

void binarytreeview::contextMenuEvent(QContextMenuEvent* event) // Q_DECL_OVERRIDE;
{
    QTreeView::contextMenuEvent(event);

    auto colInfo = -1;
    mytreeitem* selitem = this->getSelectedNodeItem(&colInfo);
    if ( colInfo != 0 ) {
        return;
    } 

    auto selRect = this->visualRect( this->selectedIndexes().at(0) );
    auto isCursorInside = selRect.isValid() && selRect.contains( event->pos() );

    if ( isCursorInside && selitem!=nullptr ) {
        // TODO : update Menu Action First
        if ( selitem!=nullptr ) {
            auto rootFlag = selitem->isRootNode();
            m_pDelteNodeAct->setEnabled(!rootFlag);

            auto nRows = selitem->rowCount();
            if ( nRows == 2) {
                m_pAddBothAct->setEnabled(false);
                m_pAddLeftAct->setEnabled(false);
                m_pAddRightAct->setEnabled(false);
            } else if ( nRows == 1 ) {
                auto child1st = selitem->child(0);
                if ( child1st!=nullptr ) {
                    auto txt = child1st->text();
                    if ( txt == "left" ) {
                        m_pAddBothAct->setEnabled(false);
                        m_pAddLeftAct->setEnabled(false);
                        m_pAddRightAct->setEnabled(true);
                    } else if ( txt == "right" ) {
                        m_pAddBothAct->setEnabled(false);
                        m_pAddLeftAct->setEnabled(true);
                        m_pAddRightAct->setEnabled(false);
                    } else {
                        m_pAddBothAct->setEnabled(false);
                        m_pAddLeftAct->setEnabled(false);
                        m_pAddRightAct->setEnabled(false);
                    }
                }
            } else if ( nRows == 0 ) {
                m_pAddBothAct->setEnabled(true);
                m_pAddLeftAct->setEnabled(true);
                m_pAddRightAct->setEnabled(true);
            }
        }

        m_pPopupMenu->popup( event->globalPos() );
        m_pPopupMenu->setFocus(Qt::PopupFocusReason);
    }
}



void binarytreeview::clearTree()
{
    if ( m_pTreeModel!=nullptr ) {
        m_pTreeModel->clear();
    }
}


void binarytreeview::initTreeView(bool needCreateRoot)
{
    // clear the data of the tree
    if ( m_pTreeModel!=nullptr ) {
        m_pTreeModel->clear();
    }

    QStringList headlst;
    headlst.push_back("name");
    headlst.push_back("value");
    headlst.push_back("status");
    m_pTreeModel->setHorizontalHeaderLabels(headlst);

    auto invisibleRoot = m_pTreeModel->invisibleRootItem();
    if ( invisibleRoot!=nullptr && needCreateRoot ) {
        const auto rootFlag = true;
        const auto canEdit = false;
        auto treeRoot = new mytreeitem("Root", rootFlag,canEdit);
        invisibleRoot->setChild(0,0, treeRoot );
        invisibleRoot->setChild(0,1, new mytreeitem("0") );
        // set invalid tag
        invisibleRoot->setChild(0,2, new mytreeitem("",false,canEdit) );
    }
}


void binarytreeview::onDeleteNode()
{
    auto colInfo = -1;
    mytreeitem* selitem = this->getSelectedNodeItem(&colInfo);
    
    if ( colInfo != 0  || selitem == nullptr || selitem->isRootNode() ) {
        return;
    }

    auto nRow = selitem->row();
    auto par = selitem->parent();
    if ( par!=nullptr ) {
        par->removeRow(nRow);
    }
}

void binarytreeview::onAdd2NodesActionTrigger()
{
    auto colInfo = -1;
    mytreeitem* selitem = this->getSelectedNodeItem(&colInfo);
    if ( colInfo != 0 || selitem == nullptr  ) {
        return;
    }

    selitem->setChild(0,0, new mytreeitem("left") );
    selitem->setChild(0,1, new mytreeitem("0") );
    selitem->setChild(1,0, new mytreeitem("right") );
    selitem->setChild(1,1, new mytreeitem("0") );
}

void binarytreeview::onAddLeftNodeActionTrigger()
{
    auto colInfo = -1;
    mytreeitem* selitem = this->getSelectedNodeItem(&colInfo);
    if ( colInfo != 0 ) {
        return;
    }
    
    selitem->insertRow(0,new mytreeitem("left"));
    selitem->setChild(0,1,new mytreeitem("0"));
}

void binarytreeview::onAddRightNodeActionTrigger()
{
    auto colInfo = -1;
    mytreeitem* selitem = this->getSelectedNodeItem(&colInfo);
    if ( colInfo != 0   ) {
        return;
    }

    auto nRows = selitem->rowCount();
    auto insertRowIdx = nRows == 0 ? 0 : 1; 

    selitem->setChild(insertRowIdx,0,new mytreeitem("right"));
    selitem->setChild(insertRowIdx,1,new mytreeitem("0"));
}

void binarytreeview::onTreeLeafItemEdited(QStandardItem* pItem)
{
    if ( pItem == nullptr || pItem->column()!=1 ) {
        return;
    }

    qDebug() << "row,col = " << pItem->row() << "," << pItem->column();
    auto strAfterChanged = pItem->text();
    // qDebug() << "strAfterChanged = " << strAfterChanged;
    strAfterChanged = strAfterChanged.trimmed();
    auto canConvertToInt = false;
    if ( strAfterChanged.isEmpty() ) {
         canConvertToInt = false;
    } else {
        strAfterChanged.toInt(&canConvertToInt);
    }

    qDebug() << "calc par ==> ";
    auto par = pItem->parent();
    if ( par == nullptr ) {
        qDebug() << "in if";
        par = m_pTreeModel->invisibleRootItem();
    } 

    qDebug() << "in else";
    // update vaild status
    if ( par!=nullptr ) {
        qDebug() << "reset data";
        QString statusText(canConvertToInt ? "" : "invalid <int>");
        // par->setChild(pItem->row(),2, nullptr); // first delete it

        // add new
        const auto isRootFlag = false;
        const auto canEditable = false;
        const auto isValid = canConvertToInt;
        auto errorItem = new mytreeitem(statusText, isRootFlag,canEditable,isValid);
        par->setChild(pItem->row(),2, errorItem);
    }
}



bool binarytreeview::prepareSavedContent(QString* pContent, QPersistentModelIndex& errorIndex)
{
    m_XmlStringList.clear();
    m_valueSet.clear();
    if( m_pTreeModel==nullptr ) {
        return false;
    }

    auto noneDisplayRoot = m_pTreeModel->invisibleRootItem();
    if ( noneDisplayRoot == nullptr ) {
        return false;
    }
    
    if ( m_pXMLDoc != nullptr ) {
        m_pXMLDoc->clear();

        rapidxml::xml_node<char>* dclNode = m_pXMLDoc->allocate_node(rapidxml::node_declaration);

        QByteArray ver("version");
        QByteArray verValue("1.0");
        QByteArray encoding("encoding");
        QByteArray encodingVal("UTF-8");
        m_XmlStringList.push_back( ver);
        m_XmlStringList.push_back( verValue);
        m_XmlStringList.push_back( encoding);
        m_XmlStringList.push_back( encodingVal);
        auto sz = m_XmlStringList.size();
        int idxLast4Ary[] = { 
            sz - 4,
            sz - 3,
            sz - 2,
            sz - 1
        };

        rapidxml::xml_attribute<char>* attr1 = m_pXMLDoc->allocate_attribute( m_XmlStringList.at(idxLast4Ary[0]).constData(), 
                                                                              m_XmlStringList.at(idxLast4Ary[1]).constData() );
        rapidxml::xml_attribute<char>* attr2 = m_pXMLDoc->allocate_attribute( m_XmlStringList.at(idxLast4Ary[2]).constData(), 
                                                                              m_XmlStringList.at(idxLast4Ary[3]).constData() );

        dclNode->append_attribute(attr1);
        dclNode->append_attribute(attr2);
        m_pXMLDoc->append_node(dclNode);
    } else {
        return false;
    }

    auto bret = travelsalTreeView(noneDisplayRoot, m_pXMLDoc, errorIndex, 0);
    if ( bret && pContent!=nullptr ) {
        // qDebug() << "m_valueSet = ";
        // for ( auto e : m_valueSet ) {
        //     qDebug() << e;
        // }

        std::string fmtStr;
        rapidxml::print( std::back_inserter(fmtStr) , *m_pXMLDoc, 0);

        // set finial data
        *pContent = QString::fromStdString(fmtStr);
    }

    return bret;
}

bool binarytreeview::loadTreeFromFile(const QString& filename, QString* pErrorStr)
{
    auto needCreateRoot = false;
    initTreeView(needCreateRoot);

    m_XmlTextByteArray.clear();
    if ( m_pXMLDoc == nullptr || m_pTreeModel == nullptr ) {
        if ( pErrorStr!=nullptr ) {
            *pErrorStr = "XML Parse or Tree Model hasn't been prepared";
        }
        return false;
    }

    QFile readFile(filename);
    if ( !readFile.open(QIODevice::ReadOnly) ) {
        if ( pErrorStr!=nullptr ) {
            *pErrorStr = QString("Can't open file %1").arg(filename);
        }
        return false;
    }
    m_XmlTextByteArray = readFile.readAll();

    auto parseRet = false;
    try {
        m_pXMLDoc->clear();
        m_pXMLDoc->parse<rapidxml::parse_full>( m_XmlTextByteArray.data() );
        
        auto noneDisplayRoot = m_pTreeModel->invisibleRootItem();
        parseRet = tryToBuildTree(noneDisplayRoot, m_pXMLDoc, pErrorStr, 0);
    } catch ( const rapidxml::parse_error& error ) {
        if ( pErrorStr!=nullptr ) {
            *pErrorStr = "Parse XML Failed";
        }
    } catch ( ... ) {
        if ( pErrorStr!=nullptr ) {
            *pErrorStr = "Parse XML Failed for unexcepted reason";
        }
    }

    return parseRet;
}


bool binarytreeview::travelsalTreeView(QStandardItem* itemNode, rapidxml::xml_node<char>* xmlparent, QPersistentModelIndex& errorIdx,int level)
{
    if ( itemNode!=nullptr && xmlparent!=nullptr ) {
        for ( int i = 0; i < itemNode->rowCount(); ++i ) {
            QString nodeName;
            QString nodeValue;
            QStandardItem* pTreeNode = nullptr;
            auto isValid = true;

            for ( int j = 0; j < 3; ++j ) {
                auto child = itemNode->child(i,j);
                auto item = dynamic_cast<mytreeitem*>( child );

                if ( item == nullptr ) {
                    errorIdx = child->index();
                    isValid = false;
                    break;
                } 

                if ( j == 0 ) {
                    pTreeNode = child;
                    nodeName = item->text();
                } else if ( j == 1 ) {
                    nodeValue = item->text();
                } else {
                    isValid = item->isValid();
                    if ( !isValid ) {
                        // qDebug() << "set invalid idx";
                        errorIdx = child->index();
                    }
                }
            }

            if ( !isValid ) {
                return false;
            } else {
                auto convertIntFlag = false;
                auto iVal = nodeValue.toInt(&convertIntFlag);
                if ( !convertIntFlag ) {
                    qDebug() << "[ERROR] : " << nodeValue << " can't convert to integer";
                    return false;
                } else {
                    auto it = m_valueSet.find(iVal);
                    if ( it != m_valueSet.end() ) {
                        qDebug() << "[ERROR] : " << nodeValue << " found the same value in the tree";
                        return false;
                    } else {
                        m_valueSet.insert(iVal);
                    }
                }


                auto ele_node = m_pXMLDoc->allocate_node(rapidxml::node_element);
                QByteArray baname;
                baname.append(nodeName);
                m_XmlStringList.push_back(baname);
                // set name
                ele_node->name( m_XmlStringList.last().constData() );

                // set name's value
                QByteArray bavalue;
                bavalue.append(nodeValue);
                m_XmlStringList.push_back(bavalue);

                auto attrValue = m_pXMLDoc->allocate_attribute("value", m_XmlStringList.last().constData() );
                ele_node->append_attribute(attrValue);
                xmlparent->append_node(ele_node);
                
                // recursive travelsal sub-child
                auto bsubvisitRet = travelsalTreeView(pTreeNode, ele_node, errorIdx,level+1);
                if ( !bsubvisitRet ) {
                    return false;
                }
            }
            
        }
    }

    return true;
}


bool binarytreeview::tryToBuildTree(QStandardItem* itemNode, rapidxml::xml_node<char>* xmlparent,QString* pErrorStr,int level)
{
    if ( itemNode == nullptr || xmlparent == nullptr ) {
        return false;
    }

    auto idx = 0;
    for ( rapidxml::xml_node<char>* child = xmlparent->first_node(); child!=nullptr; child = child->next_sibling() ) {
        auto tp = child->type();
        if ( tp != rapidxml::node_element ) {
            continue;
        }

        QStandardItem* currentItem = nullptr;
        QString tagName = child->name();
        QString gotAttrValue;
        auto attrIdx = 0;
        for ( rapidxml::xml_attribute<char>* attr = child->first_attribute(); attr!=nullptr; attr = attr->next_attribute() ) {
            QString attrname = attr->name();
            QString attrvalue = attr->value();
            if ( attrIdx == 0 ) {
                if ( attrname == "value" ) {
                    gotAttrValue = attrvalue;
                } else {
                    if ( pErrorStr!=nullptr ) {
                        *pErrorStr = "first attribute name != \"value\"";
                    }
                }
                break;
            }
        }
        
        // check value is a valid number or not
        if ( gotAttrValue.isEmpty() ) {
            if ( pErrorStr!=nullptr ) {
                *pErrorStr = "no attribute's name is \"value\" or  it is an empty string";
            }
            return false;
        }

        auto bConvertFlag = false;
        auto num = gotAttrValue.toInt(&bConvertFlag);
        if ( !bConvertFlag ) {
            if ( pErrorStr!=nullptr ) {
                *pErrorStr = QString("can't convert string %1 to number").arg(gotAttrValue);
            }
            return false;
        }

        auto isRootFlag = false;
        if ( level == 0  ) {
            if ( idx == 0 ) {
                if ( tagName != "Root" ) {
                    if ( pErrorStr != nullptr ) {
                        *pErrorStr = "First Node's name != \"Root\"";
                    }
                    return false;
                } else {
                    isRootFlag = true;
                }
            } else {
                if ( pErrorStr != nullptr ) {
                    *pErrorStr = "An well-formed XML can open have 1 child";
                }
                return false;
            }
        } else {
            if ( tagName != "left" && tagName !="right" ) {
                if ( pErrorStr != nullptr ) {
                    *pErrorStr = "sub child tag-name is neither \"left\" nor  \"right\"";
                }
                return false;
            }
        }
        

        auto canEdit = false;
        auto nodeRoot = new mytreeitem(tagName ,isRootFlag, canEdit);
        itemNode->setChild(idx,0, nodeRoot);
        auto isRootFlag2nd = false;
        canEdit = true;
        itemNode->setChild(idx,1, new mytreeitem(gotAttrValue, isRootFlag2nd,canEdit) );
        auto isRootFlag3rd = false;
        canEdit = false;
        itemNode->setChild(idx,2, new mytreeitem("", isRootFlag3rd, canEdit) );
        
        auto recursiveRet = tryToBuildTree(nodeRoot, child, pErrorStr, level+1);
        if ( !recursiveRet ) {
            return false;
        }

        ++idx;
    }
    
    return true;
}
