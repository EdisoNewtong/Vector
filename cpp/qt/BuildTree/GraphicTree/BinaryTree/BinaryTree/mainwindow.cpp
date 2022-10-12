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

	m_pScene = new QGraphicsScene(0.0 , 0.0, 800.0, 600.0, ui->graphicsView );
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
    // auto needCreateRoot = true;
    // ui->treeView->initTreeView(needCreateRoot);
}

void MainWindow::on_loadBtn_clicked()
{
    // QString	getOpenFileName(QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0)
    const auto filename = QFileDialog::getOpenFileName(this,"Load Tree From File");
    if ( filename.trimmed().isEmpty() ) {
        return;
    }

	if ( m_pTreeModel != nullptr ) {
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
	



	/*
    QString xmlcontent;
    QPersistentModelIndex errorIdx;
    auto ret = ui->treeView->prepareSavedContent(&xmlcontent,errorIdx);
    if ( !ret ) {
        // auto selmodel = ui->treeView->selectionModel();
        ui->statusBar->showMessage("Some value is invalid, Please Check", 3500);
        ui->treeView->scrollTo( static_cast<QModelIndex>(errorIdx) );
        
        // if ( selmodel!=nullptr ) {
        //     // QItemSelection qsel(errorIdx,errorIdx);
        //     // qDebug() << "selmodel != nullptr";
        //     // selmodel->select(qsel,QItemSelectionModel::Select);
        // } else {
        //     qDebug() << "selmodel == nullptr";
        // }
        return;
    }

//    qDebug() << "Parse Binary Tree Successful";
//    qDebug() << R"(content =
//==================================================)";
//    qDebug() << xmlcontent;
//    qDebug() << "==================================================)";

    // static
    auto savedfile = QFileDialog::getSaveFileName(this,"Save Tree Info" ,QString(), tr("XML files (*.xml)"));
    if ( savedfile.trimmed().isEmpty() ) {
        return;
    }

    QFile fileToSave(savedfile);
    if ( fileToSave.open(QIODevice::WriteOnly) ) {
        fileToSave.write( xmlcontent.toUtf8() );
        ui->statusBar->showMessage("Saved :)", 2500);
    } else {
        ui->statusBar->showMessage("Failed on saving file", 3500);
    }
	*/


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
	/*
    auto selmodel = ui->treeView->selectionModel();
    if ( selmodel!=nullptr ) {
        auto currentSelectedItem = selmodel->currentIndex();
        if ( !currentSelectedItem.isValid() ) {
            qDebug() << "Please Select at least 1 item! ";
            return;
        }

        // pick the item at the 0 column item
        if ( currentSelectedItem.column() != 0 ) {
            currentSelectedItem = currentSelectedItem.siblingAtColumn( 0 );
        }


        drawTreeBySelectedItem(currentSelectedItem, 0 );

    }
	*/

	using namespace GlobalSetting;


	// 5th layer
	// QGraphicsEllipseItem* ndList5th[16] = { nullptr }; 


	auto DofCircle = 2 * CIRCLE_RADIUS;

	// 4th layer
	QGraphicsEllipseItem* ndList4th[8] = { nullptr }; 
	QGraphicsEllipseItem* ndList3rd[4] = { nullptr }; 
	QGraphicsEllipseItem* ndList2nd[2] = { nullptr }; 
	QGraphicsEllipseItem* ndRoot    =   nullptr;
	for( int i = 0; i < 8; ++i ) {
		ndList4th[i] = allocCircle( CIRCLE_RADIUS, tr("%1").arg( 0 + (i+1) ) );
		ndList4th[i]->setPos( QPointF( LEFT_MARGIN + i * (DofCircle + DISTANCE_BETWEEN_LEFTRIGHT) ,  TOP_MARGIN + (4-1) * (DofCircle + HEIGHT_BETWEEN_PARENT_AND_CHILDREN) ) );
		if ( m_pScene != nullptr ) {
			m_pScene->addItem( ndList4th[i] );
		}
	}

	// 3rd layer
	for( int i = 0; i < 4; ++i ) {
		ndList3rd[i] = allocCircle( GlobalSetting::CIRCLE_RADIUS, tr("%1").arg( 8 + (i+1) ) );
        QPointF t1( LEFT_MARGIN + (1+i*2) * (DofCircle) + (( 1+4*i )/2.0)*DISTANCE_BETWEEN_LEFTRIGHT - CIRCLE_RADIUS, TOP_MARGIN + (3-1) * (DofCircle + HEIGHT_BETWEEN_PARENT_AND_CHILDREN) );
        ndList3rd[i]->setPos( t1 );

		QPointF cen = t1 + QPointF( CIRCLE_RADIUS, CIRCLE_RADIUS );

		int idxLeft  = 2*i + 0;
		int idxRight = 2*i + 1;
		QPointF cL = ndList4th[idxLeft]->pos()  + QPointF( CIRCLE_RADIUS, CIRCLE_RADIUS );
		QPointF cR = ndList4th[idxRight]->pos() + QPointF( CIRCLE_RADIUS, CIRCLE_RADIUS );

		//   QGraphicsLineItem(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent = nullptr)
		auto lineL = new QGraphicsLineItem(cL.x(), cL.y(),  cen.x(), cen.y() );
		auto lineR = new QGraphicsLineItem(cR.x(), cR.y(),  cen.x(), cen.y() ); 
		lineL->setZValue( -1.0 );
		lineR->setZValue( -1.0 );

		if ( m_pScene != nullptr ) {
			m_pScene->addItem( lineL );
			m_pScene->addItem( lineR );

			m_pScene->addItem( ndList3rd[i] );
		}
	}


	// 2nd layer
	for( int i = 0; i < 2; ++i ) {
		ndList2nd[i] = allocCircle( GlobalSetting::CIRCLE_RADIUS, tr("%1").arg( 13 + (i+1) ) );
		QPointF t1( LEFT_MARGIN + (i==0 ? 2 : 6) * DofCircle + ( (i==0 ? 3 : 11) / 2.0) * DISTANCE_BETWEEN_LEFTRIGHT - CIRCLE_RADIUS, TOP_MARGIN + (2-1) * (DofCircle + HEIGHT_BETWEEN_PARENT_AND_CHILDREN) );
		ndList2nd[i]->setPos( t1 );

		QPointF cen = t1 + QPointF( CIRCLE_RADIUS, CIRCLE_RADIUS );

		int idxLeft  = 2*i + 0;
		int idxRight = 2*i + 1;
		QPointF cL = ndList3rd[idxLeft]->pos()  + QPointF( CIRCLE_RADIUS, CIRCLE_RADIUS );
		QPointF cR = ndList3rd[idxRight]->pos() + QPointF( CIRCLE_RADIUS, CIRCLE_RADIUS );

		//   QGraphicsLineItem(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent = nullptr)
		auto lineL = new QGraphicsLineItem(cL.x(), cL.y(),  cen.x(), cen.y() );
		auto lineR = new QGraphicsLineItem(cR.x(), cR.y(),  cen.x(), cen.y() ); 
		lineL->setZValue( -1.0 );
		lineR->setZValue( -1.0 );

		if ( m_pScene != nullptr ) {
			m_pScene->addItem( lineL );
			m_pScene->addItem( lineR );

			m_pScene->addItem( ndList2nd[i] );
		}
	}

	ndRoot = allocCircle( CIRCLE_RADIUS, tr("%1").arg(19) );
    QPointF rootLeftTop( LEFT_MARGIN + 7*CIRCLE_RADIUS + 7/2.0 * DISTANCE_BETWEEN_LEFTRIGHT, TOP_MARGIN + (1-1) * (DofCircle + HEIGHT_BETWEEN_PARENT_AND_CHILDREN));
	ndRoot->setPos( rootLeftTop );

	QPointF cen = rootLeftTop + QPointF( CIRCLE_RADIUS, CIRCLE_RADIUS );

	int idxLeft  = 0;
	int idxRight = 1;
	QPointF cL = ndList2nd[idxLeft]->pos()  + QPointF( CIRCLE_RADIUS, CIRCLE_RADIUS );
	QPointF cR = ndList2nd[idxRight]->pos() + QPointF( CIRCLE_RADIUS, CIRCLE_RADIUS );

	//   QGraphicsLineItem(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent = nullptr)
	auto lineL = new QGraphicsLineItem(cL.x(), cL.y(),  cen.x(), cen.y() );
	auto lineR = new QGraphicsLineItem(cR.x(), cR.y(),  cen.x(), cen.y() ); 
	lineL->setZValue( -1.0 );
	lineR->setZValue( -1.0 );

	if ( m_pScene != nullptr ) {
		m_pScene->addItem( lineL );
		m_pScene->addItem( lineR );

		m_pScene->addItem( ndRoot );
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