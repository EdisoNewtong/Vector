#include <QDebug>
#include <QTextCursor>
#include <QList>
#include <QStringList>
#include <QFileDialog>
#include <QFile>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "JsonStandardItem.h"



#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"


static const auto basedOn0 = true;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, m_visitValue(nullptr)
	, m_pDoc(nullptr)
	, m_eleIt(nullptr)
    , m_isParseOK(false)
	, m_visitTag(E_NONE)
	, m_visitStep(-1)
	, m_pTreeModel(nullptr)
{
    ui->setupUi(this);
	m_visitStack.clear();

    ui->treeView->setStyleSheet(R"(
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
}

MainWindow::~MainWindow()
{
    clearDoc();
    clearTree();
    delete ui;
}


// Read From TextBox
void MainWindow::on_pushButton_clicked()
{
    auto txt = ui->plainTextEdit->toPlainText();
	ui->plainTextEdit_2->setPlainText("Parsing ... ");
	do_Parse(txt.toStdString().c_str());
}

// Read From File
void MainWindow::on_pushButton_2_clicked()
{
    QString pickedFileName = QFileDialog::getOpenFileName(this, QStringLiteral("Select a XML File to Read") );
    if ( pickedFileName.isNull() || pickedFileName.isEmpty() ) {
        qDebug() << "Not select";
        return;
    }

    QFile f(pickedFileName);
    if( f.open( QIODevice::ReadOnly) ) {
        auto bytes = f.readAll();
        ui->plainTextEdit->setPlainText(bytes);
    } else {
        ui->plainTextEdit_2->setPlainText(QStringLiteral("Can't Open File   \"%1\"  \n").arg(pickedFileName));
    }
    f.close();
}

// HighLight something
void MainWindow::on_pushButton_3_clicked()
{
    if( m_pDoc!=nullptr ) {
        if( m_isParseOK ) {
#ifdef EDISON_LINE_INFO_TRACE
			printValueDetail(m_visitValue, m_visitStep);
#endif
        } else {
            qDebug() << "Sorry , Parse Error , can't visit";
		}
	} 
}

// Build Tree View
void MainWindow::on_pushButton_4_clicked()
{
	if( m_isParseOK ) {
		clearTree();
		setupTreeModel();

		QStandardItem* rootNode = m_pTreeModel->invisibleRootItem();
		fillDataIntoModel(rootNode, m_pDoc, -1);	// -1 : root is Doc
	}
}


// Pretty Writer
void MainWindow::on_pushButton_5_clicked()
{
	if( m_pDoc!=nullptr && m_isParseOK ) {
		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> pretty_writer(buffer);
		m_pDoc->Accept(pretty_writer);

        const char* str = buffer.GetString();
        QString formated_prettyTxt(str);
        ui->plainTextEdit->setPlainText( formated_prettyTxt );
	} else {
		qDebug() << "Doc is nullptr or  Parse Error";
	}


}



void MainWindow::clearDoc()
{
	if( m_pDoc!=nullptr ) {
		delete m_pDoc;
		m_pDoc = nullptr;
	}
}


void MainWindow::do_Parse(const char* content)
{
	m_visitValue = nullptr;
	m_visitTag = E_NONE;
	m_visitStack.clear();

	clearDoc();
	m_pDoc = new rapidjson::Document;

    rapidjson::GenericStringStream< rapidjson::UTF8<> >::s_bQt4BytesSupportFlag = ui->checkBox->isChecked();
    // rapidjson::ParseResult ok = d.Parse( txt.toStdString().c_str() );
    rapidjson::ParseResult ok = m_pDoc->Parse( content );

	ui->plainTextEdit_2->setPlainText("");
    if( ok ) {
        // auto tp = m_pDoc->GetType();
        // qDebug() << "tp = " << tp;
		ui->plainTextEdit_2->setPlainText("Parsing Successful");
		m_isParseOK = true;

		m_visitValue = m_pDoc;
		m_visitStep = 0;	// ready to visit , Layer = 0
		m_visitTag = E_VISIT_NEXT;
    } else {
        QString errorStr = QStringLiteral("Reason = %1 \n"
                                          "Offset = %2 \n").arg(GetParseError_En(ok.Code())).arg(ok.Offset());

        #ifdef EDISON_LINE_INFO_TRACE
            auto lineinfo = ok.getLineInfo();
            QString str_ErrorlineCol_info =  QStringLiteral(" Error Msg : @ %1:%2\n").arg(lineinfo.lineNo).arg(lineinfo.colNo);
            errorStr += str_ErrorlineCol_info;
        #endif

		ui->plainTextEdit_2->setPlainText(QStringLiteral("Parsing Failed\n") +  errorStr);
		m_isParseOK = false;
		m_visitStep = -2;
		m_visitTag = E_NONE;

        highLightText( ok.getLineInfo().cursorOffset -2, ok.getLineInfo().cursorOffset);
    }
}


void MainWindow::printValueDetail(rapidjson::Value* jsonValue,int nStep)
{
#ifdef EDISON_LINE_INFO_TRACE
	if( nStep <= -1) {
		qDebug() << "Visit Finished";
		return;
	}

	if( jsonValue == nullptr ) {
		qDebug() << "nullptr json Value";
		return;
	}

    auto s = jsonValue->getLineStartInfo();
    auto e = jsonValue->getLineEndInfo();

	if( jsonValue->IsObject() || jsonValue->IsArray()  ) {
		//
		// Type == Object or Array , both is compound data type
		//

        // if( !m_visitStack.empty() ) {
		// 	if( m_visitTag == E_VISIT_OBJECT_VALUE || m_visitTag == E_VISIT_ARRAY_ELEMENT  ) {
		// 		m_tag = -1;
		// 	}
        // }

		if( jsonValue->IsObject() ) {
			// Type is Object

			// newTxt = QStringLiteral("type = Object | %1:%2 ~ %3:%4").arg(s.lineNo).arg(s.colNo).arg(e.lineNo).arg(e.colNo);
            // highLightText(s.cursorOffset, e.cursorOffset);
			////  Loop Visit
			// for(auto it = jsonValue.MemberBegin(); it!=jsonValue.MemberEnd(); ++it )
			// {
			// 	// it->name
			// 	// it->value
			// }

			// Step by Step
			// 1. {
			// 2.    member1::name
			// 3.    member1::value
			// 4.    member2::name
			// 5.    member2::value
			//     ...
			//     ...
			//     ...
			// Last.   }
		
			if( m_visitTag == E_VISIT_NEXT  ) {
				// It's the time that the type is not decide
				//
				// 1. LH --> {
                highLightText(s.cursorOffset , s.cursorOffset );
				m_visitTag = E_VISIT_OBJECT_KEY;	// next step , visit key
				m_it = jsonValue->MemberBegin();	// pointer to 1st KV member
			} else if( m_visitTag == E_VISIT_OBJECT_KEY ) {
				// LH -> Visit 1st. Key
				if( m_it != jsonValue->MemberEnd() ) {
                    // m_visitStep = nStep + 1;

                    printValueDetail(&(m_it->name), m_visitStep);   // key must be string
                    m_visitTag = E_VISIT_OBJECT_VALUE;
				} else {
					// Last but one  --> LH -> }
                    highLightText(e.cursorOffset-1 , e.cursorOffset);
					m_visitTag = E_VISIT_OBJECT_ALL;
				}
			} else if( m_visitTag == E_VISIT_OBJECT_VALUE ) {
				// Visit KV's  Value

				auto vOfKey = &(m_it->value);
				if(    vOfKey!=nullptr 
					&& (vOfKey->IsObject() || vOfKey->IsArray() ) ) {
					m_visitStep = nStep + 1;

					VisitInfo v_Info;
                    v_Info.tag = E_VISIT_OBJECT_KEY;    // next to visit next key-value of this object , visit key first
					v_Info.member_iter = m_it;
					v_Info.visitValue = jsonValue;
					m_visitStack.push( v_Info );

					m_visitValue = &(m_it->value);
                    m_visitTag = E_VISIT_NEXT;

					printValueDetail(m_visitValue, m_visitStep);
				} else {
                    printValueDetail(vOfKey, m_visitStep);
					++m_it;		// move to next
                    m_visitTag = E_VISIT_OBJECT_KEY;	// visit next key (not 1st)
				}
			} else if( m_visitTag == E_VISIT_OBJECT_ALL ) {
				// Last : LH the whole object -> { ... } 
                highLightText(s.cursorOffset , e.cursorOffset);
				m_visitStep = nStep - 1;

				if( !m_visitStack.empty() ) {
					auto topElement = m_visitStack.pop();
					if( topElement.tag == E_VISIT_OBJECT_KEY ) {
						m_it = topElement.member_iter;
						++m_it;		// move to next
						m_visitTag = E_VISIT_OBJECT_KEY;	// visit next key (not 1st)
					} else {
						m_eleIt = topElement.element_iter;
						++m_eleIt;
						m_visitTag = E_VISIT_ARRAY_ELEMENT;  // after read , set from -1 --> 4 , to read next Array Element
					}

					m_visitValue = topElement.visitValue;
                } else {
                    m_visitTag = E_VISIT_NEXT;
                }
			}
		} else {
            //
            // Type is Array
            //
			if( m_visitTag == E_VISIT_NEXT  ) {
                highLightText(s.cursorOffset , s.cursorOffset );	// LH --> [
				m_eleIt = jsonValue->Begin();	// pointer to the 1st element if existed
				m_visitTag = E_VISIT_ARRAY_ELEMENT;
			} else if( m_visitTag == E_VISIT_ARRAY_ELEMENT ) {
				if( m_eleIt != jsonValue->End() ) {
					auto eleOfArray = m_eleIt;
					if(    eleOfArray!=nullptr 
						&& (eleOfArray->IsObject() || eleOfArray->IsArray())) {
						m_visitStep = nStep + 1;

						VisitInfo v_Info;
                        v_Info.tag = E_VISIT_ARRAY_ELEMENT; // next to visit next element of this array
						v_Info.element_iter = m_eleIt;
						v_Info.visitValue = jsonValue;
						m_visitStack.push( v_Info );

						m_visitValue = m_eleIt;
                        m_visitTag = E_VISIT_NEXT;

						printValueDetail(m_eleIt, m_visitStep);
					} else {
                        printValueDetail(eleOfArray, m_visitStep);
						++m_eleIt;
						m_visitTag = E_VISIT_ARRAY_ELEMENT;  // after read , set from -1 --> 4 , to read next Array Element
					}

				} else {
                    highLightText(e.cursorOffset , e.cursorOffset);	// LH --> ]
					m_visitTag = E_VISIT_ARRAY_ALL;

				}
			} else if( m_visitTag == E_VISIT_ARRAY_ALL ) {
				// Last : LH the whole Array -> [ ... ] 
                highLightText(s.cursorOffset , e.cursorOffset);
				m_visitStep = nStep - 1;

				if( !m_visitStack.empty() ) {
					auto topElement = m_visitStack.pop();
					if( topElement.tag == E_VISIT_OBJECT_KEY ) {
						m_it = topElement.member_iter;
						++m_it;		// move to next
						m_visitTag = E_VISIT_OBJECT_KEY;	// visit next key (not 1st)
					} else {
						m_eleIt = topElement.element_iter;
						++m_eleIt;
						m_visitTag = E_VISIT_ARRAY_ELEMENT;  // after read , set from -1 --> 4 , to read next Array Element
					}

					m_visitValue = topElement.visitValue;
                } else {
                    m_visitTag = E_VISIT_NEXT;
                }
			}

		}
	} else if( jsonValue->IsNull() || jsonValue->IsBool() || jsonValue->IsString() || jsonValue->IsNumber() ) {
		// Basic Type
        highLightText(s.cursorOffset, e.cursorOffset);
		if( nStep == 0 && jsonValue == m_pDoc &&  m_visitTag != E_VISIT_OBJECT_KEY  ) {
			m_visitStep = nStep - 1;
		}
	}
#endif
}


void MainWindow::highLightText(int startpos,int endpos)
{
    qDebug() <<QStringLiteral("startpos = %1, endpos = %2").arg(startpos).arg(endpos);
	// plainTextEdit
    if( startpos>=0 &&  endpos>=0 && endpos>=startpos ) {
		auto delta = endpos - startpos;
        auto cursor = ui->plainTextEdit->textCursor();
        //
        //	Core
        //
        // 移动当前的光标到指定的位置 , 这个操作会触发 QPlainTextEdit 的滚动条的移动(如果当前的设置的光标位置不在当前的可视区域时，会立即滚动(不带动画的)到设置的位置)
        QTextCursor currentCursor(cursor);
        currentCursor.setPosition((int)startpos, QTextCursor::MoveAnchor);

        QTextEdit::ExtraSelection selection;
        cursor.setPosition(startpos-1, QTextCursor::MoveAnchor);
        cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor,  (int)(delta+1) );

        selection.cursor = cursor;
        selection.format.setBackground( QBrush(Qt::yellow) );

        QList<QTextEdit::ExtraSelection> lst;
        lst.push_back(selection);
        ui->plainTextEdit->setExtraSelections(lst);
        
        // Real move cursor , scroll-bar will move focus automatically
        ui->plainTextEdit->setTextCursor(currentCursor);
    } else {
        // qDebug() << "Can't Select Block";
    }
}



void MainWindow::clearTree()
{
	if( m_pTreeModel!=nullptr ) {
        auto selectionModel = ui->treeView->selectionModel();
        if( selectionModel!=nullptr ) {
			disconnect(selectionModel,&QItemSelectionModel::selectionChanged, this, &MainWindow::onTreeItemSelectionChanged);
            // qDebug() << "EdisonLog : Delete selectionModel : " << selectionModel;
            delete selectionModel;
        }
		ui->treeView->setModel(nullptr);

		delete m_pTreeModel;
		m_pTreeModel = nullptr;
	}
}

void MainWindow::setupTreeModel()
{
	m_pTreeModel = new  QStandardItemModel();
	QStringList headLabelList;
	headLabelList.push_back("No. / Key");
	headLabelList.push_back("Value");
	headLabelList.push_back("Type");
    m_pTreeModel->setHorizontalHeaderLabels(headLabelList);

	ui->treeView->setModel(m_pTreeModel);
	auto selectionModel = ui->treeView->selectionModel();
	if( selectionModel!=nullptr ) {
		connect(selectionModel,&QItemSelectionModel::selectionChanged, this, &MainWindow::onTreeItemSelectionChanged);
	}
	
}


void MainWindow::fillDataIntoModel(QStandardItem* parent, rapidjson::Value* jsonValue,int tag)
{
#ifdef EDISON_LINE_INFO_TRACE
    // Q_UNUSED(jsonValue);
    // Q_UNUSED(tag);

    // JSonStandardItem* wholeRoot1 = new JSonStandardItem("Root",nullptr); 
	// ("No. / Key");
	// ("Value");
	// ("Type");
    auto currentParent = parent;

	if( jsonValue==nullptr ) {
		return;
	}

    JSonStandardItem* newRoot = nullptr;
    if( tag == -1) {
        newRoot = new JSonStandardItem("Root", jsonValue);
        currentParent->setChild(0, 0, newRoot);
    }

	if( jsonValue->IsObject() || jsonValue->IsArray()  ) {
        QString type = QStringLiteral("%1").arg(jsonValue->IsObject() ? "Object" : "Array");
        if( tag == -1) {
            currentParent->setChild(0, 1, new JSonStandardItem(QStringLiteral("..."), jsonValue) );
            currentParent->setChild(0, 2, new JSonStandardItem(type, jsonValue) );
            currentParent = newRoot;
        } else {
            auto parent_of_parent = currentParent->parent();
            parent_of_parent->setChild(tag, 1, new JSonStandardItem(QStringLiteral("..."), jsonValue) );
            parent_of_parent->setChild(tag, 2, new JSonStandardItem(type, jsonValue) );
        }

        auto idx = 0;
		if( jsonValue->IsObject() ) {
            // Object
            // new JSonStandardItem(QStringLiteral("number"), jsonValue)
            // currentParent->setChild(0, 1, new JSonStandardItem(QStringLiteral("..."), jsonValue)
            idx = 0;
			for( auto iter = jsonValue->MemberBegin(); iter!=jsonValue->MemberEnd(); ++iter, ++idx) {
                // Key
                auto key_Object = &(iter->name);
                auto key_treeItem = new JSonStandardItem(key_Object->GetString() , key_Object);
                currentParent->setChild(idx,0, key_treeItem);
                
                // Value
                auto value_Object = &(iter->value);
                // auto isValueObjectArray = value_Object->IsObject() || value_Object->IsArray();
                // Core Code : Recursively
                auto isCompoundType = value_Object->IsObject() || value_Object->IsArray();
                fillDataIntoModel(isCompoundType ? key_treeItem : currentParent, value_Object, idx);
			}
		} else {
            // Array
            idx = 0;
			for(auto eleIt = jsonValue->Begin(); eleIt!=jsonValue->End(); ++eleIt, ++idx) {
                // Idx
                auto real_idx = basedOn0 ? idx : idx+1;
                auto idx_treeItem = new JSonStandardItem(QStringLiteral("%1").arg(real_idx) , eleIt);
                currentParent->setChild(idx,0, idx_treeItem);
                
                // Core Code : Recursively
                auto isCompoundType = eleIt->IsObject() || eleIt->IsArray();
                fillDataIntoModel(isCompoundType ? idx_treeItem : currentParent , eleIt, idx);
			}
		}
	} else if( jsonValue->IsNull() || jsonValue->IsBool() || jsonValue->IsString() || jsonValue->IsNumber() ) {
		// not Object / Array
        auto child_idx = (tag==-1 ? 0 : tag);
        // auto child_idx = 0;
        if( jsonValue->IsNull() ) {
            currentParent->setChild(child_idx,1, new JSonStandardItem(QStringLiteral("null") ,jsonValue));
            currentParent->setChild(child_idx,2, new JSonStandardItem(QStringLiteral("Null Type"),jsonValue));
        } else if( jsonValue->IsBool() ) {
            currentParent->setChild(child_idx,1, new JSonStandardItem(QStringLiteral("%1").arg(jsonValue->GetBool() ? "true" : "false") ,jsonValue));
            currentParent->setChild(child_idx,2, new JSonStandardItem(QStringLiteral("bool"),jsonValue));
        } else if( jsonValue->IsString() ) {
            currentParent->setChild(child_idx,1, new JSonStandardItem(QStringLiteral("%1").arg(jsonValue->GetString() ) ,jsonValue));
            currentParent->setChild(child_idx,2, new JSonStandardItem(QStringLiteral("string"), jsonValue));
        } else {
            // Type is  Number
            if( jsonValue->IsInt() ) {
                currentParent->setChild(child_idx,1, new JSonStandardItem(QStringLiteral("%1").arg(jsonValue->GetInt() ) ,jsonValue));
            } else if( jsonValue->IsUint() ) {
                currentParent->setChild(child_idx,1, new JSonStandardItem(QStringLiteral("%1").arg(jsonValue->GetUint() ) ,jsonValue));
            } else if( jsonValue->IsInt64() ) {
                currentParent->setChild(child_idx,1, new JSonStandardItem(QStringLiteral("%1").arg(jsonValue->GetInt64() ) ,jsonValue));
            } else if( jsonValue->IsUint64() ) {
                currentParent->setChild(child_idx,1, new JSonStandardItem(QStringLiteral("%1").arg(jsonValue->GetUint64() ) ,jsonValue));
            } else if( jsonValue->IsDouble() ) {
                currentParent->setChild(child_idx,1, new JSonStandardItem(QStringLiteral("%1").arg(jsonValue->GetDouble() ) ,jsonValue));
            }
            currentParent->setChild(child_idx,2, new JSonStandardItem(QStringLiteral("number"), jsonValue));
        }
	}
#endif
}


void MainWindow::onTreeItemSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    Q_UNUSED(deselected);
    if( m_pTreeModel!=nullptr ) {
        auto selectList = selected.indexes();
        if( selectList.empty() ) {
            return;
        }

        auto selectItem = dynamic_cast<JSonStandardItem*>( m_pTreeModel->itemFromIndex( selectList.at(0)   ) );
        if( selectItem!=nullptr ) {
            auto jsonValue = selectItem->getJsonValue();
            if( jsonValue!=nullptr ) {
                auto s = jsonValue->getLineStartInfo();
                auto e = jsonValue->getLineEndInfo();
                highLightText(s.cursorOffset , e.cursorOffset );
            }
        }
    }
}



