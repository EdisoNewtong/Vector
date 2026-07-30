#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileInfo>
#include <QDir>


#include <QElapsedTimer> 
#include "fnedcore.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Core Core Core :
//
//           QDir                        d("F:/Resources");
//           auto ret = d.relativeFilePath("F:/Resources/Pictures/MeAvatar/Node.jpg");
//           //   ret =                                 "Pictures/MeAvatar/Node.jpg"
//
////////////////////////////////////////////////////////////////////////////////////////////////////


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pThread( new WorkThread( this ) )
    , m_pEncDecObject( new FNEDCore(this) )
{
    ui->setupUi(this);


    connect( m_pThread, SIGNAL( updateFileListProcessState(const QString&) ), this, SLOT( on_updateFileListProcessState(const QString&) ) );
    connect( m_pThread, SIGNAL( updateFileListPrograssBar(float) ), this, SLOT( on_updateFileListPrograssBar(float) ) );
    connect( m_pThread, SIGNAL( encdecFilesFinished(const QString&) ), this, SLOT(on_encDecFinished(const QString&)) );
    connect( m_pThread, SIGNAL( aNewFileComes(const QString&) ), this, SLOT( on_encDecFinished(const QString&) ) );

    connect( m_pThread, SIGNAL(finished()), this, SLOT( onThreadDone() ) );

    connect( m_pEncDecObject, SIGNAL( updateFileContent(float) ), this, SLOT( on_updateFileContent(float) ) );

    // update ui state
    on_encBtn_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_encBtn_clicked()
{
    ui->memmapBtn->setEnabled( true );           ui->memmapBtn->setCheckState( Qt::Checked );
    ui->outputFileBtn->setEnabled( true );       ui->outputFileBtn->setCheckState( Qt::Unchecked );
    ui->buildDirOnlyBtn->setEnabled( true );     ui->buildDirOnlyBtn->setCheckState( Qt::Unchecked );
    ui->cmpMethodBtn->setEnabled( false );       ui->cmpMethodBtn->setCheckState( Qt::Unchecked );
    ui->decWithNewNameBtn->setEnabled( false );  ui->decWithNewNameBtn->setCheckState( Qt::Unchecked );
    ui->decWithCmpBtn->setEnabled( false );      ui->decWithCmpBtn->setCheckState( Qt::Unchecked );
}

void MainWindow::on_decBtn_clicked()
{
    ui->memmapBtn->setEnabled( true );          ui->memmapBtn->setCheckState( Qt::Checked );
    ui->outputFileBtn->setEnabled( false );     ui->outputFileBtn->setCheckState( Qt::Unchecked );
    ui->buildDirOnlyBtn->setEnabled( false );   ui->buildDirOnlyBtn->setCheckState( Qt::Unchecked );
    ui->cmpMethodBtn->setEnabled( false );      ui->cmpMethodBtn->setCheckState( Qt::Unchecked );
    ui->decWithNewNameBtn->setEnabled( true );  ui->decWithNewNameBtn->setCheckState( Qt::Unchecked );
    ui->decWithCmpBtn->setEnabled( true );      ui->decWithCmpBtn->setCheckState( Qt::Unchecked );

}

void MainWindow::on_cmpBtn_clicked()
{
    ui->memmapBtn->setEnabled( false );          ui->memmapBtn->setCheckState( Qt::Unchecked );
    ui->outputFileBtn->setEnabled( false );      ui->outputFileBtn->setCheckState( Qt::Unchecked );
    ui->buildDirOnlyBtn->setEnabled( false );    ui->buildDirOnlyBtn->setCheckState( Qt::Unchecked );
    ui->cmpMethodBtn->setEnabled( true );        ui->cmpMethodBtn->setCheckState( Qt::Checked );
    ui->decWithNewNameBtn->setEnabled( false );  ui->decWithNewNameBtn->setCheckState( Qt::Unchecked );
    ui->decWithCmpBtn->setEnabled( false );      ui->decWithCmpBtn->setCheckState( Qt::Unchecked );
}

void MainWindow::on_goBtn_clicked()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Test Code Only
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    int testFlag = -1;
    if ( testFlag == 1 ) {
        qDebug() << "Inside Test #1";
        return;
    } else if ( testFlag == 2 ) {
        qDebug() << "Inside Test #2";
        return;
    } else if ( testFlag == 3 ) {
        qDebug() << "Inside Test #3";
        return;
    }


    if ( ui->givenFileListBtn->checkState() != Qt::Checked ) {
        // qDebug() << "[SET] Path Done";
        m_pThread->setPath( ui->srcPath->text(), ui->destPath->text(), m_pEncDecObject );

        m_pThread->setPrintFilesOnlyFlag( ui->outputFileBtn->checkState() == Qt::Checked  );
        m_pThread->setBuildDirOnly( ui->buildDirOnlyBtn->checkState() == Qt::Checked  );

    } else {
        // TODO
        auto fileList = ui->fileListWnd->toPlainText(); (void)fileList;
    }

    if ( ui->encBtn->isChecked()  ) {
        m_pThread->setOp( WorkThread::E_ENC_OP );
    } else if ( ui->decBtn->isChecked()  ) {
        m_pThread->setOp( WorkThread::E_DEC_OP );
    } else if ( ui->cmpBtn->isChecked()  ) {
        m_pThread->setOp( WorkThread::E_CMP_OP );
    }

    // TODO:
    // set some arguments
    if ( ui->mtThreadBtn->checkState() == Qt::Checked ) {
        // Multi Thread execute
        ui->mtThreadBtn->setEnabled( false );
        if ( m_pThread->isRunning() ) {
            QString warningMsg = "Tasking is already running. ";
            qDebug() << warningMsg;
            ui->statusbar->showMessage(warningMsg , 3500);
            return;
        }
        // thread is not running now
        m_pThread->start( getThreadPriority() );
        return;
    } 

    //
    // Single Thread Here
    //
    ui->goBtn->setEnabled( false );
    m_pThread->singleThreadRun();
}



    
QThread::Priority MainWindow::getThreadPriority()
{
    auto ret = QThread::InheritPriority; // Defaully
    if ( ui->inheritPBtn->isChecked() ) {
        ret = QThread::InheritPriority;
    } else if ( ui->lowPBtn->isChecked() ) {
        ret = QThread::LowPriority;
    } else if ( ui->normalPBtn->isChecked() ) {
        ret = QThread::HighPriority;
    } else if ( ui->highPBtn->isChecked() ) {
        ret = QThread::NormalPriority;
    }

    return ret;
}

void MainWindow::onThreadDone()
{
    // qDebug() << "Task Done :)";
    ui->goBtn->setEnabled( true );
    ui->mtThreadBtn->setEnabled( true );
}



void MainWindow::on_updateFileListProcessState(const QString& msg) 
{
    ui->logWnd->appendPlainText( msg );
}

void MainWindow::on_updateFileListPrograssBar(float per)
{

    auto iVal = ui->progressTotalBar->value();
    int  i_percent = static_cast<int>( per );
    if ( i_percent > 100 ) {
        i_percent = 100;
    }

    if ( iVal != i_percent ) {
        ui->progressTotalBar->setValue( i_percent );
    }
}

void MainWindow::on_encDecFinished(const QString& msg)
{
    ui->logWnd->appendPlainText( msg );
}


void MainWindow::on_updateFileContent(float per)
{
    auto iVal = ui->progressEachBar->value();
    int  i_percent = static_cast<int>( per );
    if ( i_percent > 100 ) {
        i_percent = 100;
    }

    if ( iVal != i_percent ) {
        ui->progressEachBar->setValue( i_percent );
    }
}



