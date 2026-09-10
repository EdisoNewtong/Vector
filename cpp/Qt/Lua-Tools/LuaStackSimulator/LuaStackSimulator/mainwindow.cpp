#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "pugixml.hpp"

#include <QFileDialog>
#include <QDebug>
#include <QItemSelectionModel>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, m_pStackModel( nullptr )
{
    ui->setupUi(this);

	m_pStackModel = new TableItemModel( this );
	ui->tableView->setModel( m_pStackModel );

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_loadBtn_clicked()
{
    if( m_pStackModel == nullptr ) {
        return;
    }

	QString loadFilename = QFileDialog::getOpenFileName(this, QString("Select a file to Load ... "), QString(), QString("XML (*.xml)")   );
    QString errorMsg;
    if ( m_pStackModel->loadFromFile(loadFilename , errorMsg) ) {
        showHintsMessage( QString("Load from file '%1' Successful.").arg(loadFilename), 1, 3000 );
    } else {
        showHintsMessage( QString("[ERROR] Load Failed : %1 ").arg(errorMsg), 2, 3000 );
    }
}

void MainWindow::on_saveBtn_clicked()
{
    if( m_pStackModel == nullptr ) {
        return;
    }

	QString fileToSave = QFileDialog::getSaveFileName(this, QString("Save as ..."), QString() , QString("XML (*.xml)") );
    if ( !fileToSave.endsWith(".xml") ) {
        fileToSave += ".xml";
    }
    // qDebug() << "filePath = " << fileToSave;

    if ( m_pStackModel->saveToFile( fileToSave ) ) {
        showHintsMessage( QString("Save to '%1' Successful.").arg(fileToSave), 1, 3000 );
    } else {
        showHintsMessage( QString("Save Failed ! "), 2, 3000 );
    }

}

void MainWindow::on_clearBtn_clicked()
{
    if( m_pStackModel != nullptr ) {
        m_pStackModel->clearData();
    }

}

void MainWindow::on_optionBtn_clicked()
{

}

void MainWindow::on_picclrBtn_clicked()
{

}

void MainWindow::on_tstBtn1_clicked()
{

}

void MainWindow::on_tstBtn2_clicked()
{

}



// Add one operation onto the stack
void MainWindow::on_addBtn_clicked()
{
	if ( m_pStackModel != nullptr ) {
		m_pStackModel->addLine();
	}

}

void MainWindow::on_minusBtn_clicked()
{
    QItemSelectionModel* selectionModel = ui->tableView->selectionModel();
    if ( selectionModel != nullptr    &&   m_pStackModel != nullptr ) {
        auto sellist = selectionModel->selectedIndexes();
        if ( !sellist.empty() ) {
            QVector<int> rowSet;
            for ( auto sel : sellist ) {
                rowSet.push_back( sel.row() );
            }
            std::sort( rowSet.begin(), rowSet.end() );

            QString errorMsg;
            if ( m_pStackModel->removeLines( rowSet, errorMsg) ) {
                showHintsMessage( QString("Remove %1 row(s)").arg( rowSet.size() ), 1, 3000 );
            } else {
                showHintsMessage( QString("[ERROR] : %1").arg( errorMsg ), 2, 3000 );
            }
        } else {
            showHintsMessage( QString("No Selection"), 3, 3000 );
        }
    }

}


// void MainWindow::onStackCommandSelectionChanged(const QItemSelection& newSelection, const QItemSelection& oldSelection)
// {
// 
// }




// 0 : normal      ( Black Color )
// 1 : Successful  ( Green Color )
// 2 : Failed      ( Red Color )
// 3 : Warning     ( Brown-Orange Color )
void MainWindow::showHintsMessage( const QString& msg, int tag, int msec)
{
	if ( tag == 0 ) {
		// normal
		ui->statusbar->setStyleSheet( "QStatusBar { color: black; }" );
	} else if ( tag == 1 ) {
		// success
		ui->statusbar->setStyleSheet( "QStatusBar { color: black; background: #8fff82; }" );
	} else if ( tag == 2 ) {
		// fail
		ui->statusbar->setStyleSheet( "QStatusBar { color: red; }" );
	} else if ( tag == 3 ) {
		// warning
		ui->statusbar->setStyleSheet( "QStatusBar { color: #FFAA00; }" );
	} else {
		ui->statusbar->setStyleSheet("");
	}


	ui->statusbar->showMessage(msg, msec);
}

