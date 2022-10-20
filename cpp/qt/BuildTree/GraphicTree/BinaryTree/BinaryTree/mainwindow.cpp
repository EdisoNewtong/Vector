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
	, m_animationMsec( 1200 ) // in milliseconds
	, m_scale( 4.0 / 5 )
	, m_pTreeModel( nullptr )
	, m_btnDelegate( nullptr )
{

    ui->setupUi(this);

	GlobalSetting::resetToDefault();

	m_pScene = new QGraphicsScene(0.0 , 0.0, 800.0, 600.0, ui->graphicsView );
	m_pScene->setBackgroundBrush( GlobalSetting::scene_bg );

	ui->graphicsView->setScene( m_pScene );
	ui->graphicsView->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

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
	if ( m_pScene != nullptr ) {
		if ( m_pHighLightCircleFrame != nullptr ) {
			m_pScene->removeItem( m_pHighLightCircleFrame );
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



	if ( m_pHighLightAnimation != nullptr ) {
		m_pHighLightAnimation->stop();

		delete m_pHighLightAnimation;
		m_pHighLightAnimation = nullptr;
	}




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
		if ( m_pHighLightAnimation != nullptr ) {
			m_pHighLightAnimation->stop();
		}

		if( m_pHighLightCircleFrame != nullptr ) {
			m_pHighLightCircleFrame->setVisible( false );
			m_pScene->removeItem( m_pHighLightCircleFrame );
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
			QPointF rightbottomPt = (mostRightBottomCenterPoint + QPointF( circle_radius, circle_radius )) + QPointF(left_margin, top_margin) + QPointF(right_margin, bottom_margin);
			m_pScene->setSceneRect( QRectF( QPointF(0.0,0.0),  rightbottomPt ) );
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

        /*
        QFile fl( savedfile );
        fl.open( QIODevice::WriteOnly );
        fl.close();
        */
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
    QPainter painter;
    painter.begin(&pixmap);
    // painter.setBackground(  m_pScene->backgroundBrush() );
    painter.setBackground( GlobalSetting::scene_bg );
    painter.setRenderHints( QPainter::Antialiasing |  QPainter::TextAntialiasing );
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
	// Stop animation if it exists
	if ( m_pHighLightAnimation != nullptr ) {
		m_pHighLightAnimation->stop();
	}
	if( m_pHighLightCircleFrame != nullptr ) {
		m_pHighLightCircleFrame->setVisible( false );
        m_pScene->removeItem( m_pHighLightCircleFrame );
	}


	auto idxList = selected.indexes();
	auto deidxList = deselected.indexes();
	if ( idxList.size() != 1 || deidxList.size() != 1 ) {
		return;
	}


	//
	// set selected / unselected flag
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
				auto parent = selectedOne->parent();
				do {
					parent->select();
					parent = (parent->isRoot() ? nullptr : parent->parent());
				} while( parent!=nullptr );
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
	QGraphicsEllipseItem* pRenderCircle = nullptr;
	if ( idx.column() != 0 ) {
		auto idx0 = idx.siblingAtColumn(0);
		auto node = static_cast<treenode*>( idx0.internalPointer() );
		if ( node == nullptr ) {
			return;
		}

		pRenderCircle = node->circleObject();
		auto txt    = node->text();
		auto absDep = node->absDepth();
		auto absH   = node->absHeight();
		auto dep = node->depth();
		auto h   = node->height();
		msg = QString("node:'%1' absD=%2 absH=%3, rD=%4 rH=%5 ")
			        .arg(txt)
					.arg(absDep)
					.arg(absH)
					.arg(dep)
					.arg(h);


		if ( m_pTreeModel != nullptr ) {
			m_pTreeModel->updateSelectionIndex(idx0);
		}
	} else {
		auto node = static_cast<treenode*>( idx.internalPointer() );
		if ( node == nullptr ) {
			return;
		}

		pRenderCircle = node->circleObject();
		auto txt    = node->text();
		auto absDep = node->absDepth();
		auto absH   = node->absHeight();
		auto dep = node->depth();
		auto h   = node->height();

		msg = QString("node:'%1' absD=%2 absH=%3, rD=%4 rH=%5 ")
			        .arg(txt)
					.arg(absDep)
					.arg(absH)
					.arg(dep)
					.arg(h);
		

		if ( m_pTreeModel != nullptr ) {
			m_pTreeModel->updateSelectionIndex(idx);
		}
	}	
    ui->statusBar->showMessage(msg, 0);



	// high light the selected treeNode
	if ( pRenderCircle != nullptr ) {
        ui->graphicsView->centerOn( pRenderCircle );

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
		QColor c = pRenderCircle->brush().color();
		// qDebug() << "color.value1 = " << c;
		// QColor flipColor = QColor::fromRgb( (~(c.red()) & 0xFF), (~(c.green()) & 0xFF), (~(c.blue()) & 0xFF) );
		QColor flipColor = QColor::fromRgb( 255 - c.red(), 255 - c.green(), 255 - c.blue() );
		// qDebug() << "color.flip = " << flipColor;

        m_pHighLightCircleFrame->setPen( QPen( QBrush( flipColor ), 1.5 ) );
        m_pHighLightCircleFrame->setPos( bigCirclePos );
        m_pHighLightCircleFrame->setRect( begRect  );
		m_pHighLightCircleFrame->setVisible( true );
        m_pScene->addItem( m_pHighLightCircleFrame );

		if ( m_pHighLightAnimation == nullptr ) {
			m_pHighLightAnimation = new QPropertyAnimation( m_pHighLightCircleFrame, "rect");
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