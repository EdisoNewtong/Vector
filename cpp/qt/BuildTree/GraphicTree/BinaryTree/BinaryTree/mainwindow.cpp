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
}

MainWindow::~MainWindow()
{
	if ( m_pScene != nullptr ) {
		delete m_pScene;
		m_pScene = nullptr;
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
	QBrush outBg = GlobalSetting::scene_bg;
	auto dlg = new GlobalSettingDlg(this,&outBg); 
	dlg->exec();

	if ( m_pScene != nullptr ) {
		m_pScene->setBackgroundBrush( outBg );
	}
}




void MainWindow::on_drawTreeBtn_clicked()
{
	using namespace GlobalSetting;

	if ( m_pTreeModel != nullptr ) {
		int selectedCol = 0;
		auto pr = ui->treeView->getSelectedNodeItem(&selectedCol);
		auto selectedTreeNode = pr.second;
		if ( selectedTreeNode == nullptr ) {
			ui->statusBar->showMessage("Please Select the 1st column of the Node", 3500);
			return;
		}

        QPointF mostRightBottomCenterPoint;
        m_pTreeModel->updateDepthAndHeight( selectedTreeNode, &mostRightBottomCenterPoint);
		if ( m_pScene != nullptr ) {
			// auto rect = m_pScene->sceneRect();
			// auto width = rect.width();
			// auto height = rect.height();
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
                    QPointF lineUpperLayerPos( left_margin + node->connectionLineSelfParent_x() , top_margin + node->connectionLineSelfParent_y() );
                    auto connectionLine = new QGraphicsLineItem( left_margin + node->connectionLineSelfDot_x(), top_margin + node->connectionLineSelfDot_y(),  lineUpperLayerPos.x(), lineUpperLayerPos.y() );
					auto styleCfg = node->getNodeStyle();
					connectionLine->setPen( styleCfg.m_connectionLinePen );
                    // connectionLine->setZValue( -1.0 );

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
    // painter.setBackground(  Qt::white );
    painter.setBackground(  m_pScene->backgroundBrush() );
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
