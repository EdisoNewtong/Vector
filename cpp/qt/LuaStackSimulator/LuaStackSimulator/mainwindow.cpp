#include "mainwindow.h"
#include "ui_mainwindow.h"

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

}

void MainWindow::on_saveBtn_clicked()
{

}

void MainWindow::on_clearBtn_clicked()
{

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
}


