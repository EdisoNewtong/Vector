#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QItemSelectionModel>
#include <QDebug>
#include <QItemSelection>
#include <QFileDialog>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>


static QGraphicsEllipseItem* G_pItem = nullptr;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, m_pScene( nullptr )
{
    ui->setupUi(this);

	m_pScene = new QGraphicsScene(0.0 , 0.0, 800.0, 600.0, ui->graphicsView );
	// m_pScene->setBackgroundBrush( Qt::white ); /* Qt::blue */

	ui->graphicsView->setScene( m_pScene );
}

MainWindow::~MainWindow()
{
	if ( m_pScene != nullptr ) {
		delete m_pScene;
		m_pScene = nullptr;
	}

    delete ui;
}

void MainWindow::on_newTreeBtn_clicked()
{
    auto needCreateRoot = true;
    ui->treeView->initTreeView(needCreateRoot);
}

void MainWindow::on_loadBtn_clicked()
{
    // QString	getOpenFileName(QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0)
    const auto filename = QFileDialog::getOpenFileName(this,"Load Tree From File");
    if ( filename.trimmed().isEmpty() ) {
        return;
    }

    QString errorString;
    auto ret = ui->treeView->loadTreeFromFile(filename, &errorString);
    if ( !ret ) {
        ui->statusBar->showMessage(errorString, 3500);
    } else {
        ui->statusBar->showMessage("Load Tree OK :)", 3500);
    }
}

void MainWindow::on_saveBtn_clicked()
{
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


}

void MainWindow::on_clearBtn_clicked()
{
	// if ( m_pScene != nullptr ) {
	// 	m_pScene->update();
	// }
	
	// QGraphicsEllipseItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr)
	
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
	QPen p4( QBrush(Qt::yellow), 1);
	text3->setPen( p4 ); 
	text3->setBrush( QBrush(Qt::green) );
	text3->setPos( 220, 215 );


	m_pScene->addItem( text1 );
	m_pScene->addItem( text2 );
	m_pScene->addItem( text3 );


	/*
	   QAbstractGraphicsShapeItem                    
	    |
		 -- QGraphicsSimpleTextItem



		QGraphicsObject
		 |
		  -- QGraphicsTextItem 

	*/

}

void MainWindow::on_saveGraphicBtn_clicked()
{
	if ( G_pItem != nullptr ) {

		// QPen p( QBrush(Qt::blue), 16);
		// round2->setPen( p ); 
		// round2->setBrush( QBrush(Qt::red) );
		G_pItem->setPen( QPen( QBrush(Qt::blue), 3 ) );
		G_pItem->setBrush( QBrush(Qt::green) );
	}

}

