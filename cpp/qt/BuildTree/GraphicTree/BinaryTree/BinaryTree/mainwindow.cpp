#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QItemSelectionModel>
#include <QDebug>
#include <QItemSelection>
#include <QFileDialog>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFontMetricsF>
#include <QMessageBox>
#include <QScrollBar>
#include <QBitmap>

#include <QGraphicsLineItem>

#include "globalSettings.h"
#include "nodestylecfg.h"
#include "globalsettingdlg.h"


// static QGraphicsEllipseItem* G_pItem = nullptr;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, m_pScene( nullptr )
	, m_pHighLightCircleFrame( nullptr )
	, m_pHighLightAnimation( nullptr )
	, m_pEnumerateHVBarAni( nullptr )
	, m_pEnumerateMoveRect( nullptr )
	, m_pEnumerateMoveRectAni( nullptr )
	, m_pEnumerateStatusTextAni( nullptr )
	, m_pEnumerateTextItem( nullptr )
	, m_pEnumerateAnimationGrp( nullptr )
	, m_animationMsec( 1200 ) // in milliseconds
	, m_scale( 4.0 / 5 ) // scale factor circle when be selected in treeview
	, m_bEnumerateNodeWithAnimation( false )
	, m_pTreeModel( nullptr )
	, m_btnDelegate( nullptr )
    // , m_lastRenderedTreeRootNode( nullptr )
{

    ui->setupUi(this);

	GlobalSetting::resetToDefault();

	m_pScene = new QGraphicsScene(0.0 , 0.0, 800.0, 600.0, ui->graphicsView );
	m_pScene->setBackgroundBrush( GlobalSetting::scene_bg );

	ui->graphicsView->setScene( m_pScene );
    // ui->graphicsView->setRenderHints( QPainter::Antialiasing |  QPainter::TextAntialiasing);
    ui->graphicsView->setRenderHints( QPainter::Antialiasing |  QPainter::TextAntialiasing    | QPainter::SmoothPixmapTransform);

	m_pTreeModel = new binarytreemodel( this );
	ui->treeView->setModel( m_pTreeModel );

    m_btnDelegate = new mysettingbtndelegate( ui->treeView  );
    ui->treeView->setItemDelegateForColumn( GlobalSetting::SPECIAL_COLUMN_INDEX, m_btnDelegate );

	ui->treeView->initEventHandler();


	QItemSelectionModel* pSelModel = ui->treeView->selectionModel();
	if ( pSelModel != nullptr ) {
		// selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
		connect( pSelModel, &QItemSelectionModel::selectionChanged, this,  &MainWindow::onSelectionNodeChanged);
	}
}

MainWindow::~MainWindow()
{
	if ( m_pHighLightAnimation != nullptr ) {
		m_pHighLightAnimation->stop();
        m_pHighLightAnimation->setParent( nullptr );

        delete m_pHighLightAnimation; m_pHighLightAnimation = nullptr;
    }

	if ( m_pScene != nullptr ) {
		if ( m_pHighLightCircleFrame != nullptr ) {
			m_pScene->removeItem( m_pHighLightCircleFrame );

            delete m_pHighLightCircleFrame;
			m_pHighLightCircleFrame = nullptr;
		}

		delete m_pScene;
		m_pScene = nullptr;
	}

	QItemSelectionModel* pSelModel = ui->treeView->selectionModel();
	if ( pSelModel != nullptr ) {
		delete pSelModel;
		pSelModel = nullptr;
	}
	

	if ( m_pTreeModel != nullptr ) {
		ui->treeView->setModel( nullptr );
		delete m_pTreeModel;
		m_pTreeModel = nullptr;
	}

	if ( m_btnDelegate != nullptr ) {
		// detach  delegate already existed    immediately
		ui->treeView->setItemDelegate( nullptr );
		
        delete m_btnDelegate;
        m_btnDelegate = nullptr;
	}


	stopEnuerateAnimationIfNecessary(true);

    delete ui;
}

void MainWindow::on_newTreeBtn_clicked()
{
	auto makeSureCreateNewTree = true;
	if ( m_pTreeModel != nullptr && !m_pTreeModel->isTreeOnlyHasRoot() ) {
		auto ret = QMessageBox::question(this, tr("Discard Current Tree ?"), 
				                    tr("The tree already has left/right nodes.\nAre you sure destroy it and rebuild a new tree ?") );

		makeSureCreateNewTree = (ret == QMessageBox::Yes);
	}

	if ( makeSureCreateNewTree ) {
		 m_pTreeModel->generateANewTree();
	}
}

void MainWindow::on_loadBtn_clicked()
{
    // QString	getOpenFileName(QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0)

	if ( m_pTreeModel != nullptr ) {
		auto makeSureCreateNewTree = true;
		if ( !m_pTreeModel->isTreeOnlyHasRoot() ) {
			auto ret = QMessageBox::question(this, tr("Discard Current Tree ?"), 
										tr("The tree already has left/right nodes.\nAre you sure destroy it and load a new tree from file ?") );

			makeSureCreateNewTree = (ret == QMessageBox::Yes);
		}

		if ( !makeSureCreateNewTree ) {
			return;
		}

		const auto filename = QFileDialog::getOpenFileName(this,"Load Tree From File");
		if ( filename.trimmed().isEmpty() ) {
			return;
		}

		QString errorMsg;

		if ( m_pTreeModel->loadFromFile(filename, &errorMsg) ) {
			ui->statusBar->showMessage("Load Tree OK :)", 3500);
            // update Scene's Background
            m_pScene->setBackgroundBrush( GlobalSetting::scene_bg );
			ui->treeView->expandAll();
		} else {
            ui->statusBar->showMessage(errorMsg, 3500);
		}
	}

}

void MainWindow::on_saveBtn_clicked()
{
	
    auto savedfile = QFileDialog::getSaveFileName(this,"Save Binary Tree" ,QString(), tr("XML files (*.xml)"));
    if ( savedfile.trimmed().isEmpty() ) {
        return;
    }

	if ( m_pTreeModel != nullptr ) {
		QString errorMsg;
		QPersistentModelIndex errIdx;
        if ( m_pTreeModel->saveToFile(savedfile, &errorMsg, errIdx) ) {
			ui->statusBar->showMessage("Save Tree OK :)", 3500);
		} else {
            ui->statusBar->showMessage(errorMsg, 3500);

			auto selmodel = ui->treeView->selectionModel();
			if ( selmodel != nullptr ) {
				QItemSelection selRange( errIdx, errIdx );
				selmodel->select( selRange,  QItemSelectionModel::Select);
				ui->treeView->scrollTo( static_cast<QModelIndex>(errIdx) );
			}

		}
	}
	

}

void MainWindow::on_clearBtn_clicked()
{
	if ( m_pScene != nullptr ) {
		stopEnuerateAnimationIfNecessary(true);

        if ( m_pHighLightCircleFrame!=nullptr ) {
			m_pScene->removeItem( m_pHighLightCircleFrame );

            delete m_pHighLightCircleFrame;
            m_pHighLightCircleFrame = nullptr;
        }

		m_pScene->clear();
		m_pScene->update( m_pScene->sceneRect() );

		if ( m_pTreeModel != nullptr ) {
			m_pTreeModel->updateDepthAndHeightForRoot();
			const auto& nodevec = m_pTreeModel->getTreeNodes();
            for( auto nd = nodevec.begin(); nd != nodevec.end(); ++nd ) {
				treenode* node = *nd;
				if ( node != nullptr ) {
					node->detachAllRenderObject();
				}
			}
		}


	}

}

void MainWindow::on_treeOptionBtn_clicked()
{
	auto dlg = new GlobalSettingDlg(this); 
	dlg->exec();

	if ( m_pScene != nullptr ) {
		m_pScene->setBackgroundBrush( GlobalSetting::scene_bg );
	}
}




void MainWindow::on_drawTreeBtn_clicked()
{
	using namespace GlobalSetting;
	on_clearBtn_clicked();

	if ( m_pTreeModel != nullptr ) {
		int selectedCol = 0;
		auto pr = ui->treeView->getSelectedNodeItem(&selectedCol);
		auto selectedTreeNode = pr.second;
		if ( selectedTreeNode == nullptr ) {
			ui->statusBar->showMessage("Please Select the 1st column of the Node", 3500);
			return;
		}


        QPointF mostRightBottomCenterPoint;
		m_pTreeModel->updateGlobalTree();
        m_pTreeModel->updateDepthAndHeight( selectedTreeNode, &mostRightBottomCenterPoint);
		if ( m_pScene != nullptr ) {
			auto d = 2.0 * circle_radius;
			QPointF rightbottomPt = (mostRightBottomCenterPoint + QPointF( circle_radius, circle_radius )) + QPointF(left_margin, top_margin) + QPointF(right_margin, bottom_margin);
			m_pScene->setSceneRect( QRectF( QPointF(0.0, 0.0),  rightbottomPt ) );

			auto hBar = ui->graphicsView->horizontalScrollBar();
			if ( hBar != nullptr ) {
				hBar->setPageStep( static_cast<int>(d) );
			}

			auto vBar = ui->graphicsView->verticalScrollBar();
			if ( vBar != nullptr ) {
				vBar->setPageStep( static_cast<int>(d) );
			}
			
			// qDebug() << "rightbottomPt = " << rightbottomPt;
		}



		const auto& nodevec = m_pTreeModel->getTreeNodes();
		for( auto nd = nodevec.begin(); nd != nodevec.end(); ++nd ) 
		{
			treenode* node = *nd;
			if ( node != nullptr ) {
				// alloc circle with text by the passed node struct
				auto circleTextPr = allocCircle( node );
				auto circle = circleTextPr.first;
				auto text = circleTextPr.second;
				QPointF centerPt( left_margin + node->x(), top_margin + node->y() );
				QPointF leftTop = centerPt - QPointF(circle_radius, circle_radius);
				
				if ( circle!=nullptr   &&   text!=nullptr ) {
					circle->setPos( leftTop );
					// Core Core Core : attach render circle with text
					node->setCircle(circle);
					node->setTextObject(text);

					m_pScene->addItem( circle );
				}
				
				// render the connection line
				if ( selectedTreeNode != node ) {
                    QPointF lineUpperLayerPos( left_margin + node->connectionLineParent_x() , top_margin + node->connectionLineParent_y() );
                    auto connectionLine = new QGraphicsLineItem( left_margin + node->connectionLineSelfDot_x(), top_margin + node->connectionLineSelfDot_y(),  lineUpperLayerPos.x(), lineUpperLayerPos.y() );
					auto styleCfg = node->getNodeStyle();
					connectionLine->setPen( styleCfg.m_connectionLinePen );

					// Core Core Core : attach render connection line
					node->setLine( connectionLine );
					m_pScene->addItem( connectionLine );
				}
			}
			
		}

		// center focus on the selected node 
		auto rootCircleObject = selectedTreeNode->circleObject();
		if ( rootCircleObject != nullptr ) {
			ui->graphicsView->centerOn( rootCircleObject );
		}
	}
	


}




void MainWindow::on_saveGraphicBtn_clicked()
{
	// qDebug() << "To Save Picture";
    auto savedfile = QFileDialog::getSaveFileName(this,"Save Tree As png", QString(), tr("Png Files (*.png)") );
    if ( savedfile.trimmed().isEmpty() ) {
        ui->statusBar->showMessage("Cancel Saving File", 3500);
        return;
    } else {
        savedfile = savedfile.trimmed();

    }

    //
    // Core Core Core :
    //
    //      Must set the pixmap's size first , otherwise , the save-file operation will be failed by the following Error List
    //
    // QPainter::begin: Paint device returned engine == 0, type: 2
    // QPainter::setBackground: Painter not active
    // QPainter::setRenderHint: Painter must be active to set rendering hints
    // QPainter::save: Painter not active

    auto rectSz = m_pScene->sceneRect().size().toSize();

    QPixmap pixmap( rectSz );
    // in order to save a transparent background 
    //  You must do the following code to set the mask of the pixmap
    QBitmap mask( rectSz );
    mask.fill( Qt::black );
    pixmap.setMask( mask );

    QPainter painter;
    painter.setBackgroundMode(  Qt::TransparentMode );
    painter.setBackground( GlobalSetting::scene_bg );
    painter.setRenderHints( QPainter::Antialiasing |  QPainter::TextAntialiasing );

    painter.begin(&pixmap);
    // painter.setBackground(  m_pScene->backgroundBrush() );
      m_pScene->render(&painter);
    painter.end();

    auto succ = pixmap.save(savedfile,"PNG");
    if ( !succ ) {
        QString errorMsg("Save Png Failed");
        ui->statusBar->showMessage(errorMsg, 3500);
        QMessageBox::critical(this, QStringLiteral("Saving Error"), errorMsg);
    } else {
        QMessageBox::information(this, QStringLiteral("Saving Status"), QStringLiteral("Png File Saved") );
        ui->statusBar->showMessage("Saving File Done", 3500);
    }



}



QPair<QGraphicsEllipseItem*, QGraphicsSimpleTextItem*> MainWindow::allocCircle(treenode* node)
{
	if ( node == nullptr ) {
		return qMakePair(nullptr, nullptr);
	}

    auto styleCfg = node->getNodeStyle();

	using namespace GlobalSetting;
	auto d = 2.0 * circle_radius;
	auto graphicCircle =  new QGraphicsEllipseItem(0, 0, d, d);
	graphicCircle->setPen( styleCfg.m_circlePen  ); 
	graphicCircle->setBrush( styleCfg.m_circleBrushColor );

	// CIRCLE_BRUSH
	auto graphicText = new QGraphicsSimpleTextItem( node->text() , graphicCircle );
	graphicText->setFont( styleCfg.m_textFont );
	graphicText->setBrush( styleCfg.m_textBrush );

	auto fnt = graphicText->font();

	QFontMetricsF fmObj( fnt );
    auto dWidth = fmObj.horizontalAdvance( node->text() );
	auto dHeight = fmObj.height();
	auto x = (d - dWidth) / 2.0;
	auto y = (d - dHeight) / 2.0;

	graphicText->setPos( QPointF(x,y) );

	// graphicCircle->addItem( graphicText );
	return qMakePair( graphicCircle, graphicText); 
}


void MainWindow::onSelectionNodeChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    // bool bLaterDeleteFlag = false;
	// Stop animation if it exists
	if ( m_pHighLightAnimation != nullptr ) {
		m_pHighLightAnimation->stop();
        m_pHighLightAnimation->setParent( nullptr );

        /*
         
           Because the animation's target is m_pHighLightCircleFrame
           So it can't be destroied at once

           after invoke the following code;
                delete m_pHighLightAnimation;


           its binded parent named 'm_pHighLightCircleFrame' will be destroied  in the deleteLater pool

           so the following code 'm_pHighLightCircleFrame' if ( m_pHighLightCircleFrame!=nullptr )   will return true

           But m_pHighLightCircleFrame will be delete later , its memory will be referenced  m_pHighLightCircleFrame->setVisible( false );


        */
        // bLaterDeleteFlag = true;
        delete m_pHighLightAnimation;
        m_pHighLightAnimation = nullptr;
	}

	if( m_pHighLightCircleFrame != nullptr ) {
		m_pHighLightCircleFrame->setVisible( false );
        m_pScene->removeItem( m_pHighLightCircleFrame );

        //////////////////////////////////////////////////////////////////////////////////////////////
        //
        // DON'T delete it because , it will be automatically deleted by    'm_pHighLightAnimation'
        //
        //////////////////////////////////////////////////////////////////////////////////////////////
        delete m_pHighLightCircleFrame;
		m_pHighLightCircleFrame = nullptr;
	}

    // if ( bLaterDeleteFlag ) {
    //     delete m_pHighLightAnimation;
    //     m_pHighLightAnimation = nullptr;
    // }


	auto idxList = selected.indexes();
	auto deidxList = deselected.indexes();
	if ( idxList.size() != 1 || deidxList.size() != 1 ) {
		return;
	}


	//
	// set selected / unselected flag
	QString selectedNodePath;
	QVector<treenode*> nodevec;
	if ( m_pTreeModel != nullptr ) {
		m_pTreeModel->updateDepthAndHeightForRoot();
		nodevec = m_pTreeModel->getTreeNodes();
		for( auto nd = nodevec.begin(); nd != nodevec.end(); ++nd ) {
			treenode* node = *nd;
			if ( node != nullptr ) {
				node->unselect();
			}
		}

		auto deidx = deidxList.at(0);
		if ( deidx.column() != 0 ) {
			auto de0 = deidx.siblingAtColumn(0);
			m_pTreeModel->updateSelectionIndex( de0 );
		} else {
			m_pTreeModel->updateSelectionIndex( deidx );
		}

		treenode* selectedOne = nullptr;
		auto idx = idxList.at(0);
		if ( idx.column() != 0 ) {
			auto idx0 = idx.siblingAtColumn(0);
			selectedOne = static_cast<treenode*>( idx0.internalPointer() );
		} else {
			selectedOne = static_cast<treenode*>( idx.internalPointer() );
		}

		if ( selectedOne != nullptr ) {
			auto hasPicked = false;
			for( auto nd = nodevec.begin(); nd != nodevec.end(); ++nd ) {
				treenode* node = *nd;
				if ( node == selectedOne ) {
					hasPicked = true;
					node->select();
					break;
				}
			}

			if ( hasPicked && !selectedOne->isRoot() ) {
				selectedNodePath = ( selectedOne->isLeftNode() ? "left" : "right" );
				auto parent = selectedOne->parent();
				while ( parent != nullptr ) {
					parent->select();
					if ( parent->isRoot() ) {
						parent = nullptr;
						selectedNodePath.prepend("root -> "); 
					} else {
						if ( parent->isLeftNode() ) {
							selectedNodePath.prepend("left -> "); 
						} else {
							selectedNodePath.prepend("right -> "); 
						}

						parent = parent->parent();
					}
				} 
			} else {
				selectedNodePath = "root";
			}


			if ( idx.column() != 0 ) {
				auto idx0 = idx.siblingAtColumn(0);
				m_pTreeModel->updateSelectionIndex( idx0 );
			} else {
				m_pTreeModel->updateSelectionIndex( idx );
			}

		}

	}


	auto idx = idxList.at(0);
	QString msg;
	QString selectNodeText;
	QGraphicsEllipseItem* pRenderCircle = nullptr;
	if ( idx.column() != 0 ) {
		auto idx0 = idx.siblingAtColumn(0);
		auto node = static_cast<treenode*>( idx0.internalPointer() );
		if ( node == nullptr ) {
			return;
		}

		pRenderCircle = node->circleObject();
		selectNodeText = node->text();
		auto absDep = node->absDepth();
		auto absH   = node->absHeight();
		auto dep = node->depth();
		auto h   = node->height();
		msg = QString("node:'%1' absD=%2 absH=%3, rD=%4 rH=%5 | %6")
			        .arg(selectNodeText)
					.arg(absDep)
					.arg(absH)
					.arg(dep)
					.arg(h)
					.arg( selectedNodePath );


	} else {
		auto node = static_cast<treenode*>( idx.internalPointer() );
		if ( node == nullptr ) {
			return;
		}

		pRenderCircle = node->circleObject();
		selectNodeText = node->text();
		auto absDep = node->absDepth();
		auto absH   = node->absHeight();
		auto dep = node->depth();
		auto h   = node->height();

		msg = QString("node:'%1' absD=%2 absH=%3, rD=%4 rH=%5 | %6")
			        .arg(selectNodeText)
					.arg(absDep)
					.arg(absH)
					.arg(dep)
					.arg(h)
					.arg( selectedNodePath );
		

	}	
    ui->statusBar->showMessage(msg, 0);



	// high light the selected treeNode
	if ( pRenderCircle != nullptr ) {
		// Edison TestOnly
        ui->graphicsView->centerOn( pRenderCircle );
		qDebug() << QString("centerOn Node : '%1'.pos = ").arg(selectNodeText) << pRenderCircle->pos();

        auto d     = 2.0 * GlobalSetting::circle_radius;
		auto h_gap = GlobalSetting::height_between_parent_and_children;

		auto pos     = pRenderCircle->pos();
		auto bigCirclePos = pos - QPointF(h_gap * m_scale,  h_gap * m_scale );
		auto newR = GlobalSetting::circle_radius + h_gap * m_scale;


		QRectF    begRect(0,0, 2.0*newR, 2.0*newR);
        QRectF    endRect( h_gap * m_scale, h_gap * m_scale, d, d);

		if ( m_pHighLightCircleFrame == nullptr ) {
			m_pHighLightCircleFrame = new mygraphicscircleitem( );
			m_pHighLightCircleFrame->setBrush( Qt::NoBrush );
        } 

		QColor c = GlobalSetting::scene_bg.color();
		// QColor flipColor = QColor::fromRgb( (~(c.red()) & 0xFF), (~(c.green()) & 0xFF), (~(c.blue()) & 0xFF) );
		QColor flipColor = QColor::fromRgb( 255 - c.red(), 255 - c.green(), 255 - c.blue() );

        m_pHighLightCircleFrame->setPen( QPen( QBrush( flipColor ), 1.5 ) );
        m_pHighLightCircleFrame->setPos( bigCirclePos );
        m_pHighLightCircleFrame->setRect( begRect  );
		m_pHighLightCircleFrame->setVisible( true );
        m_pScene->addItem( m_pHighLightCircleFrame );

		if ( m_pHighLightAnimation == nullptr ) {
			m_pHighLightAnimation = new QPropertyAnimation(m_pHighLightCircleFrame, "rect");
		}

		m_pHighLightAnimation->setDuration( m_animationMsec );
		m_pHighLightAnimation->setStartValue( begRect );
		m_pHighLightAnimation->setEndValue( endRect );

		// qDebug() << "Do Animation ... ";
		// loop forever
		m_pHighLightAnimation->setLoopCount( -1 );
		m_pHighLightAnimation->start();

	}
}


void MainWindow::on_preOrderBtn_clicked()
{
	static const auto flag1 = 1;
	auto selNode = getSelectedNode();
	if ( selNode == nullptr  ) {
		return;
	}

    auto bRunAnimation = ui->animationChkBtn->isChecked();
	stopEnuerateAnimationIfNecessary();

	if ( m_pTreeModel != nullptr ) {

		QVector<treenode*> outputList;
		if ( !m_pTreeModel->preOrderTravelsal( selNode, outputList ) ) {
			return;
		}

		enumerateNodes(flag1, outputList, bRunAnimation);
	}
}

void MainWindow::on_inOrderBtn_clicked()
{
	static const auto flag2 = 2;
	auto selNode = getSelectedNode();
	if ( selNode == nullptr  ) {
		return;
	}

    auto bRunAnimation = ui->animationChkBtn->isChecked();
	stopEnuerateAnimationIfNecessary();

	if ( m_pTreeModel != nullptr ) {
		QVector<treenode*> outputList;
		if ( !m_pTreeModel->inOrderTravelsal( selNode, outputList ) ) {
			return;
		}

		enumerateNodes(flag2, outputList, bRunAnimation);
	}
	
}

void MainWindow::on_postOrderBtn_clicked()
{
	static const auto flag3 = 3;
	auto selNode = getSelectedNode();
	if ( selNode == nullptr  ) {
		return;
	}

    auto bRunAnimation = ui->animationChkBtn->isChecked();
	stopEnuerateAnimationIfNecessary();

	if ( m_pTreeModel != nullptr ) {

		QVector<treenode*> outputList;
		if ( !m_pTreeModel->postOrderTravelsal( selNode, outputList ) ) {
			return;
		}

		enumerateNodes(flag3, outputList, bRunAnimation);
	}
}

treenode* MainWindow::getSelectedNode()
{
	treenode* retNode = nullptr;
	QItemSelectionModel* pSelModel = ui->treeView->selectionModel();
	if ( pSelModel != nullptr ) {
		auto idxList = pSelModel->selection().indexes();
		if ( !idxList.empty() ) {
			auto idx = idxList.at(0);
			if ( idx.column() == 0 ) {
				retNode = static_cast<treenode*>( idx.internalPointer() );
			} else {
				// not 
				retNode = static_cast<treenode*>( idx.siblingAtColumn(0).internalPointer() );
			}
		}
	}

    return retNode;
}

void MainWindow::enumerateNodes(int type, const QVector<treenode*>& nodeList, bool bAnimationFlag)
{
	m_bEnumerateNodeWithAnimation = bAnimationFlag;

	QString retStr;
	if ( type == 1 ) {
		retStr = "Pre-Order Travelsal  : ";
	} else if ( type == 2 ) {
		retStr = "In-Order Travelsal   : ";
	} else if ( type == 3 ) {
		retStr = "Post-Order Travelsal : ";
	}


	if ( !bAnimationFlag ) {
		for( auto i = 0; i < nodeList.size(); ++i ) {
			auto nd = nodeList.at(i);
			if ( nd != nullptr ) {
				retStr += nd->text();
				if ( i < (nodeList.size()-1) ) {
					retStr += " -> ";
				}
			}
		}

		ui->travelsalOutput->setText( retStr );
	} else {
		qDebug() << "Run Animation ...";

		int sz = nodeList.size();
		if ( sz <= 1 ) {
			ui->travelsalOutput->setText( retStr );
			return;
		} 

		// sz > 1
		int duration = ui->animationSpeed->value();
		if ( duration <= 0 ) {
			duration = 3500;
		}

		if ( m_pEnumerateAnimationGrp == nullptr ) {
			m_pEnumerateAnimationGrp = new QParallelAnimationGroup();
		}


		//
		// set HV Bar animation
		//
        if ( m_pEnumerateHVBarAni == nullptr ) {
			m_pEnumerateHVBarAni = new QPropertyAnimation( ui->graphicsView, "hvValue" );
		}


		auto node1st = nodeList.at(0);
		auto circle1st = node1st->circleObject();
		if ( node1st != nullptr ) {
            ui->graphicsView->centerOn( circle1st );
		}

		m_pEnumerateHVBarAni->setDuration( duration * sz );
		auto previousPercent = 0.0;
        for( auto i = 0; i < sz; ++i ) {
			auto nd = nodeList.at(i);
			if ( nd != nullptr ) {
				bool bIsOK = false;
				QPoint leftTopPt = getHVScrollBarValueForMakeCircleCenter(nd->circleObject() , &bIsOK);
				if ( bIsOK ) {
					// m_pEnumerateHVBarAni->setKeyValueAt( 1.0/(sz-1) * i, leftTopPt );


					if ( i == 0  ) {
						previousPercent = 1.0/(sz-1) * i;
						m_pEnumerateHVBarAni->setKeyValueAt( previousPercent, leftTopPt );
					} else {
						auto currentEnd = 1.0/(sz-1) * i;
						auto step1 = (previousPercent + currentEnd) / 2.0;
						auto step2 = currentEnd;
						m_pEnumerateHVBarAni->setKeyValueAt(step1, leftTopPt);
						m_pEnumerateHVBarAni->setKeyValueAt(step2, leftTopPt);

						previousPercent = step2;
					}
				}
			}
		}




		auto d = GlobalSetting::circle_radius * 2.0;
        auto edgeLength = d * 1.5;
		auto delta = edgeLength - d;
		if ( m_pEnumerateMoveRect == nullptr ) {
			m_pEnumerateMoveRect = new mygraphicsrectitem();
		}

		if ( circle1st != nullptr ) {
			QColor c = GlobalSetting::scene_bg.color();
			QColor flipColor = QColor::fromRgb( 255 - c.red(), 255 - c.green(), 255 - c.blue() );

			QPointF wrapRectBegPos = circle1st->pos() - QPointF( delta/2.0, delta/2.0);
            QRectF begRect(0,0, edgeLength, edgeLength);
			m_pEnumerateMoveRect->setBrush( Qt::NoBrush );
			m_pEnumerateMoveRect->setPen( QPen( QBrush( flipColor ), 1.5 ) );
			m_pEnumerateMoveRect->setPos( wrapRectBegPos );
			m_pEnumerateMoveRect->setRect( begRect  );
			m_pEnumerateMoveRect->setVisible( true );

			m_pScene->addItem( m_pEnumerateMoveRect );
		}


		//
		// set BoundingBox Rect Movement
		//
		if ( m_pEnumerateMoveRectAni == nullptr ) {
			m_pEnumerateMoveRectAni = new QPropertyAnimation(m_pEnumerateMoveRect ,"pos");
		}

		m_pEnumerateMoveRectAni->setDuration( duration * sz );
		previousPercent = 0.0;
        for( auto i = 0; i < sz; ++i ) {
			auto nd = nodeList.at(i);
			if ( nd != nullptr ) {
				auto pRenderCircle =  nd->circleObject();
				if ( pRenderCircle != nullptr ) {
					QPointF wrapPos = pRenderCircle->pos() - QPointF( delta/2.0, delta/2.0);
					// m_pEnumerateMoveRectAni->setKeyValueAt( 1.0/(sz-1) * i, wrapPos );

					if ( i == 0   ) {
						previousPercent = 1.0/(sz-1) * i;
						double middlePercent = 1.0/(sz-1) / 2.0;
						
						m_pEnumerateMoveRectAni->setKeyValueAt( previousPercent, wrapPos );
						m_pEnumerateMoveRectAni->setKeyValueAt( middlePercent,   wrapPos );
						previousPercent = middlePercent;
					} else {
						auto currentEnd = 1.0/(sz-1) * i;
						auto step1 = (previousPercent + currentEnd) / 2.0;
						auto step2 = currentEnd;
						m_pEnumerateMoveRectAni->setKeyValueAt( step1, wrapPos );
						m_pEnumerateMoveRectAni->setKeyValueAt( step2, wrapPos );

						previousPercent = step2;
					}
				}
			}
		}



		// set html prefix string
		if ( type == 1 ) {
			retStr = "<span>Pre-Order Travelsal  : </span>";
		} else if ( type == 2 ) {
			retStr = "<span>In-Order Travelsal   : </span>";
		} else if ( type == 3 ) {
			retStr = "<span>Post-Order Travelsal : </span>";
		}
		// txtlist << retStr;


		//
		// set Animation output html text
		//
		if ( m_pEnumerateTextItem == nullptr ) {
			m_pEnumerateTextItem = new MyTextAnimationItem( ui->travelsalOutput, nodeList, retStr);
		}

		if ( m_pEnumerateStatusTextAni == nullptr ) {
			m_pEnumerateStatusTextAni  = new QPropertyAnimation(m_pEnumerateTextItem, "step");
		}


		// QString previousText;
		previousPercent = 0.0;
		m_pEnumerateStatusTextAni->setDuration( duration * sz );
        for( auto i = 0; i < sz; ++i ) {
			auto nd = nodeList.at(i);
			if ( nd != nullptr ) {
				if ( i == 0  ) {
					previousPercent = 1.0/(sz-1) * i;
					m_pEnumerateStatusTextAni->setKeyValueAt( previousPercent, QPoint(i,i) );
				} else {
					auto currentEnd = 1.0/(sz-1) * i;
					auto step1 = (previousPercent + currentEnd) / 2.0;
					auto step2 = currentEnd;
					m_pEnumerateStatusTextAni->setKeyValueAt( step1, QPoint(i,1) );
					m_pEnumerateStatusTextAni->setKeyValueAt( step2, QPoint(i,0) );

					previousPercent = step2;
				}

			}
		}


		//
		// Run Group Animation
		//
		m_pEnumerateAnimationGrp->addAnimation( m_pEnumerateHVBarAni );
		m_pEnumerateAnimationGrp->addAnimation( m_pEnumerateMoveRectAni );
		m_pEnumerateAnimationGrp->addAnimation( m_pEnumerateStatusTextAni );
		m_pEnumerateAnimationGrp->start();
	}

}

void MainWindow::stopEnuerateAnimationIfNecessary(bool needForce /* = false */)
{
	if ( needForce || m_bEnumerateNodeWithAnimation ) {
		// qDebug() << "----- Stop previous Begin ----- ";
		if ( m_pEnumerateHVBarAni != nullptr ) {
			m_pEnumerateHVBarAni->stop();

			delete m_pEnumerateHVBarAni;
			m_pEnumerateHVBarAni = nullptr;
		}

		if ( m_pEnumerateMoveRect != nullptr ) {
			m_pEnumerateMoveRect->setVisible( false) ;
			m_pScene->removeItem( m_pEnumerateMoveRect );
			m_pEnumerateMoveRect = nullptr;
		}


		if ( m_pEnumerateMoveRectAni != nullptr ) {
			m_pEnumerateMoveRectAni->stop();

			delete m_pEnumerateMoveRectAni;
			m_pEnumerateMoveRectAni = nullptr;
		}

		if( m_pEnumerateTextItem != nullptr ) {
			delete m_pEnumerateTextItem;
			m_pEnumerateTextItem = nullptr;
		}


		if ( m_pEnumerateStatusTextAni != nullptr ) {
			m_pEnumerateStatusTextAni->stop();

			delete m_pEnumerateStatusTextAni;
			m_pEnumerateStatusTextAni = nullptr;
		}


		if ( m_pEnumerateAnimationGrp != nullptr ) {
			m_pEnumerateAnimationGrp->stop();

			delete m_pEnumerateAnimationGrp;
			m_pEnumerateAnimationGrp = nullptr;
		}
		// qDebug() << "----- Stop previous End ----- ";

	}
}








QPoint MainWindow::getHVScrollBarValueForMakeCircleCenter(QGraphicsEllipseItem* pCircle, bool* pIsOK)
{
	QPoint pt(0.0,  0.0);
	if ( pCircle == nullptr ) {
		if ( pIsOK != nullptr ) {
			*pIsOK = false;
		}
		return pt;
	}

	if ( pIsOK != nullptr ) {
		*pIsOK = true;
	}

	auto tweakDeltaFlag = false;

	// render circle measurement
	auto pos = pCircle->pos();
	auto d = GlobalSetting::circle_radius * 2.0;

	// scroll view's content size 
    auto viewSz = ui->graphicsView->size();
	auto viewW = viewSz.width();
    auto viewH = viewSz.height();
	auto centerHalf_HGap = (viewW - d) / 2.0;
	auto centerHalf_VGap = (viewH - d) / 2.0;

	// render scene's size
	auto sceneRect = ui->graphicsView->sceneRect();
	auto sceneW = sceneRect.width();
	auto sceneH = sceneRect.height();

	auto leftSpace = pos.x();
	auto rightSpace = sceneW - (leftSpace + d);
	auto topSpace = pos.y();
	auto bottomSpace = sceneH - (topSpace + d);

	auto scrollPlacedX = 0.0;
	auto scrollPlacedY = 0.0;

	// Key Core :
	//
	// Calc  Position
	//
	int hTag = 0;
	if ( leftSpace >= centerHalf_HGap    &&   rightSpace >= centerHalf_HGap ) {
		// the Horizontal position can be center
		scrollPlacedX = leftSpace - centerHalf_HGap;
		hTag = 1;
	} else {
		if ( leftSpace >= centerHalf_HGap ) {
			// leftSpace is enough  , rightSpace is not enough
			// scrollPlacedX = leftSpace - ( viewW - (rightSpace + d) );
			auto hBar = ui->graphicsView->horizontalScrollBar();
			if ( hBar != nullptr ) {
				scrollPlacedX = hBar->maximum();
			} else {
				scrollPlacedX = leftSpace - ( viewW - (rightSpace + d) );
			}

			hTag = 2;
		} else {
			// rightSpace  is enough , leftSpace is not enough
			scrollPlacedX = 0.0;
			hTag = 3;
		}
	}

	if ( tweakDeltaFlag ) {
		if ( hTag == 1 ) {
			auto deltaH = 0.0;
			auto hPageStep = 0;
			auto hBar = ui->graphicsView->horizontalScrollBar();
			if ( hBar != nullptr ) {
				hPageStep = hBar->pageStep();
				// qDebug() <<" hPageStep = " << hPageStep;
				deltaH = (viewW - hPageStep) / 2.0;
			}

			if ( scrollPlacedX > 0.0 ) {
				scrollPlacedX += deltaH;
			}
		}
	}


	int vTag = 0;
	if ( topSpace >= centerHalf_VGap  &&    bottomSpace >= centerHalf_VGap ) {
		scrollPlacedY = topSpace - centerHalf_VGap;
		vTag = 1;
	} else {
		if ( topSpace >= centerHalf_HGap ) {
			// topSpace  is enough  , bottomSpace is not enough
			auto vBar = ui->graphicsView->verticalScrollBar();
			if ( vBar != nullptr ) {
				scrollPlacedY = vBar->maximum();
			} else {
				scrollPlacedY = topSpace - ( viewH - (bottomSpace + d) );
			}
			vTag = 2;
		} else {
			// topSpace  is not enough  , bottomSpace is enough
			scrollPlacedY = 0.0;
			vTag = 3;
		}
	}

	if ( tweakDeltaFlag ) {
		if ( vTag == 1 ) {
			auto deltaV = 0.0;
			auto vPageStep = 0;
			auto vBar = ui->graphicsView->verticalScrollBar();
			if ( vBar != nullptr ) {
				vPageStep = vBar->pageStep();
				// qDebug() <<" vPageStep = " << vPageStep;
				deltaV = (viewH - vPageStep) / 2.0;
			}

			if ( scrollPlacedY > 0.0 ) {
				scrollPlacedY += deltaV;
			}
		}
	}


	pt.setX( static_cast<int>(scrollPlacedX) );
	pt.setY( static_cast<int>(scrollPlacedY) );

	return pt;
}



void MainWindow::on_stopAnimationBtn_clicked()
{
	stopEnuerateAnimationIfNecessary( true );
}

void MainWindow::on_applyOptionBtn_clicked()
{
    if ( m_pTreeModel!=nullptr ) {
        int selectedCol = 0;
		auto selectedNode = ui->treeView->getSelectedNodeItem(&selectedCol).second;
        if ( selectedNode == nullptr ) {
            selectedNode = m_pTreeModel->getRootNode();
        }

        if( selectedNode != nullptr ) {
            m_pTreeModel->setNodeStyle_preOrderTravelsal_by_givenNode( selectedNode );
        }

        // reRenderPreRootTree();
    }
}

void MainWindow::reRenderPreRootTree()
{
	using namespace GlobalSetting;
    on_drawTreeBtn_clicked();
}





void MainWindow::on_genCodeBtn_clicked()
{
    if ( m_pTreeModel!=nullptr ) {
		int selectedCol = 0;
		auto pr = ui->treeView->getSelectedNodeItem(&selectedCol);
		auto selectedTreeNode = pr.second;
        QString code = m_pTreeModel->genCode_by_givenNode( selectedTreeNode );
        ui->codeEditor->setPlainText( code );
    }

}



