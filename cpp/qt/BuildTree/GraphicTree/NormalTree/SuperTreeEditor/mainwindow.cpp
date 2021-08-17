#include <QDebug>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TreeModel.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto treemodel = new TreeModel(this);
    ui->tableView->setModel( treemodel );
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
    // 

    auto treemodel = static_cast<TreeModel*>( ui->tableView->model() );
    if ( treemodel == nullptr ) {
        ui->statusBar->showMessage("No Tree Model , Can't perform <Save> operator", 3500);
        return;
    }

    QString xmlcontent;
    QModelIndex errorIdx;
    auto ret = treemodel->prepareSavedString(xmlcontent, errorIdx);
    if ( !ret ) {
        // auto selmodel = ui->treeView->selectionModel();
        ui->statusBar->showMessage("Some tree node's value is invalid, Please Check", 3500);
        ui->tableView->scrollTo( errorIdx );
        return;
    }

    // static
    auto savedfile = QFileDialog::getSaveFileName(this,"Save Tree Info");
    if ( savedfile.trimmed().isEmpty() ) {
        return;
    }

    QFile fileToSave(savedfile);
    if ( fileToSave.open(QIODevice::WriteOnly) ) {
        fileToSave.write( xmlcontent.toUtf8() );
        ui->statusBar->showMessage("Entire Tree Info Saved :)", 2500);
    } else {
        ui->statusBar->showMessage("Failed on saving file", 3500);
    }
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

