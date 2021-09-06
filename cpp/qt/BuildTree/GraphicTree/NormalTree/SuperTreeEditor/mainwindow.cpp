#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

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
    ui->treeView->setModel( m_myTreeModel );

    connect( m_myTreeModel, SIGNAL( forceSetCheckBoxByLoadedFile(int) ), this, SLOT( on_forceSetCheckBoxState(int) )  );
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_loadTreeFromFileBtn_clicked()
{
    auto fileToLoaded = QFileDialog::getOpenFileName( this, QStringLiteral("Select a XML file to open "), QString(), QString("XML Files (*.xml)") );

    auto treemodel = static_cast<TreeModel*>( ui->treeView->model() );
    if ( treemodel == nullptr ) {
        ui->statusBar->showMessage("No Tree Model , Can't perform <Save> operator", 3500);
        return;
    }


    QString errorMsg;
    auto bret = treemodel->loadFileIntoTreeView(fileToLoaded,  errorMsg );
    if ( !bret ) {

        ui->statusBar->showMessage(errorMsg, 3500);
        QMessageBox::critical(this,QStringLiteral("Load File Error"), errorMsg);
        return;
    }

    // else Load XML File Successful
    ui->treeView->expandAll();

}

void MainWindow::on_saveTreeToFileBtn_clicked()
{
    auto  isSupportNumberOnly = ui->checkBoxOption0->checkState() == Qt::Checked;
    if ( m_myTreeModel != nullptr ) {
        m_myTreeModel->setSupportNumberOnlyFlag( isSupportNumberOnly );
    }

    ui->checkBoxOption0->setEnabled(false);

    auto treemodel = static_cast<TreeModel*>( ui->treeView->model() );
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
        ui->treeView->scrollTo( errorIdx );

        ui->checkBoxOption0->setEnabled(true);
        return;
    }

    // static
    auto savedfile = QFileDialog::getSaveFileName(this,"Save Tree Info", QString(), tr("XML Files (*.xml)") );
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



void MainWindow::on_forceSetCheckBoxState(int checkedState)
{
    ui->checkBoxOption0->setCheckState( static_cast<Qt::CheckState>( checkedState ) );
}
