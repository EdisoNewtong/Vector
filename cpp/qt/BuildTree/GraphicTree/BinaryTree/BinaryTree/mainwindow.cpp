#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QItemSelectionModel>
#include <QDebug>
#include <QItemSelection>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
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
        ui->treeView->scrollTo((QModelIndex) errorIdx);
        
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
    auto savedfile = QFileDialog::getSaveFileName(this,"Save Tree Info");
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

}

void MainWindow::on_saveGraphicBtn_clicked()
{

}



