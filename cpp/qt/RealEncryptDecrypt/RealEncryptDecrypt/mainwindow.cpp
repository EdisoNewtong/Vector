#include <QFile>
#include <QDebug>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}


MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_clicked()
{
	ui->plainTextEdit->setPlainText( QStringLiteral("") );
	//QString QFileDialog::getOpenFileName(QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0)
	auto fname = QFileDialog::getOpenFileName(this, QStringLiteral("Title A")  );
	ui->lineEdit->setText(fname);

    // auto fname = ui->plainTextEdit->toPlainText();
    QFile f(fname);
    if( f.open( QIODevice::ReadOnly ) ) {
        auto original_Content = f.readAll();
		ui->plainTextEdit->setPlainText( QString(original_Content) );
	} else {
        const QString errorMsg =  QStringLiteral("Open File \"%1\" Failed").arg(fname);
		ui->statusBar->showMessage( errorMsg );
		ui->plainTextEdit->setPlainText( errorMsg );
	}

}


void MainWindow::test()
{
    //
    // default compress level = 6
    //
	
    auto fname = ui->plainTextEdit->toPlainText();
    QFile f(fname);
    if( f.open( QIODevice::ReadOnly ) ) {
        auto original_Content = f.readAll();
        const auto SZ = 11;
        int levelAry[SZ];
        levelAry[0] = -1;
        for ( int i = 1; i<SZ; ++i) {
            levelAry[i] = i-1;
        }

        QByteArray* pAry[SZ] = { nullptr };
        for( int i = 0; i < SZ; ++i ) {
            pAry[i] = nullptr;
        }

        for ( int i = 0; i < SZ; ++i) {
            int level = levelAry[i];
            pAry[i] = new QByteArray( qCompress(original_Content, level) );
        }


        if( pAry[0]!=nullptr ) {
            for ( int i = 1; i < SZ; ++i) {
                if( pAry[i]!=nullptr &&  ((*(pAry[0])) == (*(pAry[i]))) ) {
                    qDebug() << QStringLiteral("compressed level %1 == %2").arg(levelAry[i]).arg(-1 );
                }
            }
        }

        if( pAry[1]!=nullptr ) {
            // level 0
            if( (*(pAry[1])) == original_Content ) {
                qDebug() << QStringLiteral("compressed leve %1 == original_Content").arg(levelAry[1]);
            }
        }



        for( int i = 0; i < SZ; ++i ) {
            if( pAry[i] != nullptr) {
                delete pAry[i];
                pAry[i] = nullptr;
            }
        }

    } else {
        qDebug() << "!!! Read File Failed";
    }
}

void MainWindow::test2()
{

}
