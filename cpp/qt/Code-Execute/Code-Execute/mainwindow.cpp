#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "tokenMgr.h"
#include "variblePool.h"
#include "tokenMgr.h"
using namespace std;


#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    DataTypeUtil::init();
    VariblePool::init();
    TokenMgr::init();

}

MainWindow::~MainWindow()
{

    TokenMgr::release();
    VariblePool::finalize();
    DataTypeUtil::finalize();


    delete ui;
}

