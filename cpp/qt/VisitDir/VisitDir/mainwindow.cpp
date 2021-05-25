#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "visitThread.h"

#include <QDir>
#include <QFileInfoList>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	// , m_runningThread(nullptr)
	, m_runningStatus(0)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
	QString path2Visit = ui->visitPath->toPlainText();
	qDebug() << "path2Visit = " << path2Visit;
	QDir tst(path2Visit);
    const auto& lst = tst.entryInfoList();
	qDebug() << "lst.size() = " << lst.size();
	for ( int i = 0; i < lst.size(); ++i ) {
		auto info = lst.at(i);
		qDebug() << "\t" << (i+1) << ". " << info.path();
	}


	/*
	//
	// Already running
	//
	if ( m_runningStatus == 1 ) {
		return;
	} 

	// m_runningStatus != 0
	ui->outputBox->setPlainText("");
	m_runningStatus = 1;

	QString path2Visit = ui->visitPath->toPlainText();
	auto workthread = new VisitThread();
	// void onStartVisit(const QString &dPath,int type, int level);
	connect(workthread, &VisitThread::onStartVisit, this, &MainWindow::onVisitSomething );
	connect(workthread, &VisitThread::onVisitDone, this,  &MainWindow::onVisitDirDone );
    connect(workthread, &VisitThread::finished, workthread, &QObject::deleteLater );
	workthread->setVisitPath(path2Visit);
	workthread->start();
	*/
}


void MainWindow::onVisitSomething(const QString &what, int type, int level)
{
	auto oldstr = ui->outputBox->toPlainText();
	QString visitTpName;
	if ( type == 0 ) {
		visitTpName = "Dir";
	} else if ( type == 1 ) {
		visitTpName = "File";
	} else {
		visitTpName = "Unknown-File";
	}
	auto newstr = oldstr + "\n" + QString("%1-<%2> : %3").arg(level).arg(visitTpName).arg(what);
	ui->outputBox->setPlainText(newstr);
}


void MainWindow::onVisitDirDone()
{
	m_runningStatus = 0;
}


