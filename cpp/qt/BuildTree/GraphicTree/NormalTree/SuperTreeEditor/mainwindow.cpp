#include <QDebug>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TreeModel.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_myTreeModel( nullptr )
{
    ui->setupUi(this);

    m_myTreeModel = new TreeModel(this);
    ui->tableView->setModel( m_myTreeModel );
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
    auto  isSupportNumberOnly = ui->checkBoxOption0->checkState() == Qt::Checked;
    if ( m_myTreeModel != nullptr ) {
        m_myTreeModel->setSupportNumberOnlyFlag( isSupportNumberOnly );
    }

    ui->checkBoxOption0->setEnabled(false);
    // 
    auto treemodel = static_cast<TreeModel*>( ui->tableView->model() );
    if ( treemodel == nullptr ) {
        ui->statusBar->showMessage("No Tree Model , Can't perform <Save> operator", 3500);
        ui->checkBoxOption0->setEnabled(true);
        return;
    }

    QString xmlcontent;
    QModelIndex errorIdx;
    auto ret = treemodel->prepareSavedString(xmlcontent, errorIdx);
    if ( !ret ) {
        // auto selmodel = ui->treeView->selectionModel();
        ui->statusBar->showMessage("Some tree node's value is invalid, Please Check", 3500);
        ui->tableView->scrollTo( errorIdx );

        ui->checkBoxOption0->setEnabled(true);
        return;
    }

    // static
    auto savedfile = QFileDialog::getSaveFileName(this,"Save Tree Info");
    if ( savedfile.trimmed().isEmpty() ) {
        ui->checkBoxOption0->setEnabled(true);
        return;
    }

    QFile fileToSave(savedfile);
    if ( fileToSave.open(QIODevice::WriteOnly) ) {
        fileToSave.write( xmlcontent.toUtf8() );
        ui->statusBar->showMessage("Entire Tree Info Saved :)", 2500);
    } else {
        ui->statusBar->showMessage("Failed on saving file", 3500);
    }
    ui->checkBoxOption0->setEnabled(true);
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


void MainWindow::on_checkBoxOption0_stateChanged(int arg1)
{
    auto isSupportNumberOnly = (arg1 == Qt::Checked);
    if ( m_myTreeModel != nullptr ) {
        m_myTreeModel->setSupportNumberOnlyFlag( isSupportNumberOnly );
    }
}

