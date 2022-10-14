#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QItemSelectionModel>
#include <QDebug>
#include <QItemSelection>
#include <QFileDialog>
#include <QPen>
#include <QBrush>
#include <QFontMetricsF>
#include <QGraphicsLineItem>
#include <QMessageBox>

#include "globalSettings.h"


// static QGraphicsEllipseItem* G_pItem = nullptr;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, m_pScene( nullptr )
	, m_pTreeModel( nullptr )
	, m_btnDelegate( nullptr )
{
    ui->setupUi(this);

	m_pScene = new QGraphicsScene(0.0 , 0.0, 1600.0, 900.0, ui->graphicsView );
	// m_pScene->setBackgroundBrush( Qt::white ); /* Qt::blue */

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
	}

}

void MainWindow::on_treeOptionBtn_clicked()
{

}



/******************************************************************************** 
Core Core Core :

     Draw the tree recursively function

level : the depth of a given item ( 0 for the root item )
*********************************************************************************/
void MainWindow::drawTreeBySelectedItem(const QModelIndex& selected, int level)
{
	(void)selected;
    (void)level;

	// Get the text
	auto textField = selected.siblingAtColumn(1);
	if ( !textField.isValid() ) {
		return;
	}

	auto content = textField.data().toString();

	auto graphicsCircleWithContent = allocCircle( GlobalSetting::CIRCLE_RADIUS, content );
	if ( m_pScene != nullptr  &&    graphicsCircleWithContent != nullptr ) { 
        graphicsCircleWithContent->setPos( QPointF(10,10) );
		m_pScene->addItem( graphicsCircleWithContent );
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
			qDebug() << "Selected Node is invalid";
			return;
		}

		m_pTreeModel->updateDepthAndHeight( selectedTreeNode );
		const auto& nodevec = m_pTreeModel->getTreeNodes();

		for( auto nd = nodevec.begin(); nd != nodevec.end(); ++nd ) 
		{
			treenode* node = *nd;
			if ( node != nullptr ) {
				auto graphicsCircleWithContent = allocCircle( CIRCLE_RADIUS , node->text() );
				QPointF centerPt( LEFT_MARGIN + node->x(), TOP_MARGIN + node->y() );
				QPointF leftTop = centerPt - QPointF(CIRCLE_RADIUS, CIRCLE_RADIUS);
				
				graphicsCircleWithContent->setPos( leftTop );
				m_pScene->addItem( graphicsCircleWithContent );
				
				if ( selectedTreeNode != node ) { // !node->isRoot() 
					auto parentNode = node->parent();
					if ( parentNode != nullptr ) {
						QPointF parentCenter( LEFT_MARGIN +  parentNode->x(), TOP_MARGIN + parentNode->y() );
						auto connectionLine = new QGraphicsLineItem( centerPt.x(), centerPt.y(),  parentCenter.x(), parentCenter.y() );
						connectionLine->setZValue( -1.0 );
						m_pScene->addItem( connectionLine );
					}
				}

			}
			
		}
	}
	


}




void MainWindow::on_saveGraphicBtn_clicked()
{
	qDebug() << "To Save Picture";


	/*
	auto tag = 122;
	if (  tag == 123 ) {
		return;
	}

	auto round1 = new QGraphicsEllipseItem( 10, 10, 200, 200);
	round1->setPen( QPen(Qt::black) ); 
	round1->setBrush( QBrush(Qt::red) );

	auto round2 = new QGraphicsEllipseItem( 220, 10, 200, 200);
	QPen p( QBrush(Qt::blue), 16);
	round2->setPen( p ); 
	round2->setBrush( QBrush(Qt::red) );
	G_pItem = round2;

	auto round3 = new QGraphicsEllipseItem( 220, 115, 200, 200);
	QPen p33( QBrush(Qt::blue), 5);
	round3->setPen( p33 ); 
	round3->setBrush( QBrush(Qt::red) );

	auto round4 = new QGraphicsEllipseItem( 10, 115, 200, 200);
	round4->setPen( QPen(Qt::black) );
	round4->setBrush( QBrush(Qt::red) );


	m_pScene->addItem(round1);
	m_pScene->addItem(round2);
	m_pScene->addItem(round3);
	m_pScene->addItem(round4);

	auto text1 = new QGraphicsSimpleTextItem("ABC");
	QPen p2( QBrush(Qt::yellow), 2);
	text1->setPen( p2 ); 
	text1->setBrush( QBrush(Qt::green) );
	text1->setPos( 10, 110 );

	auto text2 = new QGraphicsSimpleTextItem("DEF");
	QPen p3( QBrush(Qt::yellow), 1);
	text2->setPen( p3 ); 
	text2->setBrush( QBrush(Qt::green) );
	text2->setPos( 220, 110 );

	auto text3 = new QGraphicsSimpleTextItem("DEF");
	QFont fnt( text3->font() );
	qDebug() << tr("old size = %1").arg( fnt.pointSize() );
	fnt.setPointSize( 16 );
	text3->setFont( fnt );
	// QPen p4( QBrush(Qt::yellow), 1);
	// text3->setPen( p4 ); 
	text3->setBrush( QBrush(Qt::green) );
	text3->setPos( 240, 215 );


	m_pScene->addItem( text1 );
	m_pScene->addItem( text2 );
	m_pScene->addItem( text3 );
	
	*/


	/*
	   QAbstractGraphicsShapeItem                    
	    |
		 -- QGraphicsSimpleTextItem



		QGraphicsObject
		 |
		  -- QGraphicsTextItem 

	*/


	/*
	if ( G_pItem != nullptr ) {

		// QPen p( QBrush(Qt::blue), 16);
		// round2->setPen( p ); 
		// round2->setBrush( QBrush(Qt::red) );
		G_pItem->setPen( QPen( QBrush(Qt::blue), 3 ) );
		G_pItem->setBrush( QBrush(Qt::green) );
	}
	*/



}



QGraphicsEllipseItem* MainWindow::allocCircle(const qreal& r, const QString& text)
{
	auto d = 2.0 * r;
	auto graphicCircle =  new QGraphicsEllipseItem(0, 0, d, d);
	graphicCircle->setPen( GlobalSetting::CIRCLE_OUTLINE ); 
	graphicCircle->setBrush( GlobalSetting::CIRCLE_BRUSH );

	// CIRCLE_BRUSH
	auto graphicText = new QGraphicsSimpleTextItem( text, graphicCircle );
	graphicText->setFont( GlobalSetting::TEXT_FONT );
	graphicText->setBrush( GlobalSetting::TEXT_COLOR );

	auto fnt = graphicText->font();

	QFontMetricsF fmObj( fnt );
	auto dWidth = fmObj.horizontalAdvance(text);
	auto dHeight = fmObj.height();
	auto x = (d - dWidth) / 2.0;
	auto y = (d - dHeight) / 2.0;

	graphicText->setPos( QPointF(x,y) );

	// graphicCircle->addItem( graphicText );
	return graphicCircle; 
}