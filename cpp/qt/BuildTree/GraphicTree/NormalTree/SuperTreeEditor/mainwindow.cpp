#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TreeModel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableView->setModel( new TreeModel() );
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_loadTreeFromFileBtn_clicked()
{

}

void MainWindow::on_saveTreeToFileBtn_clicked()
{

}

void MainWindow::on_clearTreeBtn_clicked()
{

}

void MainWindow::on_drawTreeBtn_clicked()
{

}

void MainWindow::on_saveAsPngBtn_clicked()
{

}
