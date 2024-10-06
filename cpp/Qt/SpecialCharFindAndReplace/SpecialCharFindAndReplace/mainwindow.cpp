#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();
}

void MainWindow::initUI()
{
    ui->infoBox->hide();

    QStringList labels{ QString("Enable"), QString("Tag"), QString("Char"), QString("Variants-Char-List"), QString("Sep"), QString("Detail") };
    ui->cfgTableWidget->setHorizontalHeaderLabels(labels);

    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_ENABLE_STATE), 30);
    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_TAG_NAME),56);
    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_ORIGINAL_CHAR), 35);
    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_VARIANT_CHAR_LIST), 90);
    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_SEP_CHAR), 35);
    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_DETAIL_INFO), 80);

    ui->cfgTableWidget->resizeColumnsToContents();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_loadFileBtn_clicked()
{

}

void MainWindow::on_saveFileBtn_clicked()
{

}

void MainWindow::on_sPrevBtn_triggered(QAction *arg1)
{
    Q_UNUSED(arg1)
}

void MainWindow::on_sNextBtn_triggered(QAction *arg1)
{
    Q_UNUSED(arg1)
}

void MainWindow::on_searchBtn_clicked()
{

}

void MainWindow::on_replace1Btn_clicked()
{

}

void MainWindow::on_replaceAllBtn_clicked()
{

}

void MainWindow::on_newCfgBtn_clicked()
{

}

void MainWindow::on_newRowBtn_clicked()
{

}

void MainWindow::on_loadCfgBtn_clicked()
{

}

void MainWindow::on_saveCfgBtn_clicked()
{

}

void MainWindow::on_checkLogicBtn_clicked()
{

}

void MainWindow::on_actionShow_InfoBox_toggled(bool arg1)
{
    if ( arg1 ) {
        ui->infoBox->show();
    } else {
        ui->infoBox->hide();
    }
}
