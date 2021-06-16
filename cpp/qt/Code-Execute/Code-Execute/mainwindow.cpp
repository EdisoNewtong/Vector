#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, m_MetaTypeFactory()
{
    ui->setupUi(this);

	const auto pIntMeta = m_MetaTypeFactory.getMetaInfo( numberEnumNS::TYPE_ID::E_UNSIGNED_INT );
	qDebug() << "int.type = " << static_cast<int>(pIntMeta->m_Type);
}

MainWindow::~MainWindow()
{
    delete ui;
}

