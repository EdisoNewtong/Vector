#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileInfo>
#include <QDir>


#include <QElapsedTimer> 
#include "fnedcore.h"

/****************************************************************************************************

xxxx->setEnabled( t | f );
xxxx->setCheckState( Qt::Checked || Qt::Unchecked);


ui->mtThreadBtn
ui->memmapBtn
ui->outputFileBtn       // visit a given dir and output the dir-tree information    
ui->givenFileListBtn    // process target files from the given file list inside     ui->fileListWnd
ui->buildDirOnlyBtn     // traversal the target dir -> and build the same empty dir structure followed by the target dir
ui->decWithNewNameBtn   // Decrypt the source file  -> new destination file ( rather than decrypt the given file -> original with the same file name )
ui->decWithRenameBtn    // dji_123_dec.WMV          ->  dji_123.MP4


ui->processedFileEdit->setText();

****************************************************************************************************/




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
    connect( m_pThread, SIGNAL( printDirTree(const QString&) ),  this, SLOT( on_printFileList(const QString&) ) );

    connect( m_pThread, SIGNAL(finished()), this, SLOT( onThreadDone() ) );

    connect( m_pEncDecObject, SIGNAL( updateFileContent(float) ), this, SLOT( on_updateFileContent(float) ) );
    connect( m_pEncDecObject, SIGNAL(update_fileName_processInfo(const QString& ) ), this, SLOT( on_updateProcessingFileName(const QString&) ) );

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
     ui->srcPath->setEnabled( true );
     ui->destPath->setEnabled( true );
     ui->fileListWnd->setEnabled( false );

     ui->cmpSplitBtn->setText( "Execute Encrypt");
     ui->cmpSplitBtn->setEnabled( true );        ui->cmpSplitBtn->setCheckState( Qt::Checked );
    ui->decWithNewNameBtn->setEnabled( false );  ui->decWithNewNameBtn->setCheckState( Qt::Unchecked );
    ui->decWithRenameBtn->setEnabled( false );   ui->decWithRenameBtn->setCheckState( Qt::Unchecked );
}

void MainWindow::on_decBtn_clicked()
{
    ui->memmapBtn->setEnabled( true );          ui->memmapBtn->setCheckState( Qt::Checked );
    ui->outputFileBtn->setEnabled( false );     ui->outputFileBtn->setCheckState( Qt::Unchecked );
    ui->buildDirOnlyBtn->setEnabled( false );   ui->buildDirOnlyBtn->setCheckState( Qt::Unchecked );

    ui->cmpSplitBtn->setText("---");
     ui->cmpSplitBtn->setEnabled( false );       ui->cmpSplitBtn->setCheckState( Qt::Unchecked );

    ui->decWithNewNameBtn->setEnabled( true );  ui->decWithNewNameBtn->setCheckState( Qt::Unchecked );
    ui->decWithRenameBtn->setEnabled( true );   ui->decWithRenameBtn->setCheckState( Qt::Unchecked );

}

void MainWindow::on_cmpBtn_clicked()
{
    ui->memmapBtn->setEnabled( true );           ui->memmapBtn->setCheckState( Qt::Unchecked );
    ui->outputFileBtn->setEnabled( false );      ui->outputFileBtn->setCheckState( Qt::Unchecked );

    // disconnect( ui->givenFileListBtn, SIGNAL( stateChanged(int)) , this, SLOT( on_givenFileListBtn_stateChanged(int) ) );
    ui->givenFileListBtn->setEnabled( true );    ui->givenFileListBtn->setCheckState( Qt::Unchecked );
     ui->srcPath->setEnabled( true );
     ui->destPath->setEnabled( true );
     ui->fileListWnd->setEnabled( false );
     ui->fileListWnd->setPlaceholderText("Input 2 file paths on 2 inputboxs : src && dest. ");
    
    ui->buildDirOnlyBtn->setEnabled( false );    ui->buildDirOnlyBtn->setCheckState( Qt::Unchecked );

    // disconnect( ui->cmpSplitBtn, SIGNAL( stateChanged(int)) , this, SLOT( on_cmpSplitBtn_stateChanged(int) ) );
    ui->cmpSplitBtn->setText("Compare Split by ;");
    ui->cmpSplitBtn->setEnabled( false );         ui->cmpSplitBtn->setCheckState( Qt::Unchecked );
    //connect(    ui->cmpSplitBtn, SIGNAL( stateChanged(int)) , this, SLOT( on_cmpSplitBtn_stateChanged(int) ) );

    ui->decWithNewNameBtn->setEnabled( false );  ui->decWithNewNameBtn->setCheckState( Qt::Unchecked );
    ui->decWithRenameBtn->setEnabled( false );   ui->decWithRenameBtn->setCheckState( Qt::Unchecked );
}

/****************************************************************************************************

    Go Button Logic

****************************************************************************************************/
void MainWindow::on_goBtn_clicked()
{
    if ( m_pThread->isRunning() ) { 
        // xxx->isRunning() is only checked if multithread is running
        ui->statusbar->showMessage( QStringLiteral("Tasking is already running. "), 3500);
        return;
    }


    if ( ui->encBtn->isChecked()  ) {
        // Enc Mode
        m_pThread->setOp( WorkThread::E_ENC_OP );

        auto bDoEncryptFlag = (ui->cmpSplitBtn->checkState() == Qt::Checked);
        m_pThread->setTake_EncryptActionFlag( bDoEncryptFlag );
        // set args
        if ( ui->givenFileListBtn->checkState() == Qt::Checked ) {
            m_pThread->fillFileList( ui->fileListWnd->toPlainText() );
        } else {
            m_pThread->setPath( ui->srcPath->text(), ui->destPath->text(), m_pEncDecObject );

            m_pThread->setPrintFilesOnlyFlag( ui->outputFileBtn->checkState() == Qt::Checked  );
            m_pThread->setBuildDirOnly( ui->buildDirOnlyBtn->checkState() == Qt::Checked  );
        }

    } else if ( ui->decBtn->isChecked()  ) {
        // Dec Mode
        m_pThread->setOp( WorkThread::E_DEC_OP );

        // set args
        if ( ui->givenFileListBtn->checkState() == Qt::Checked ) {
            m_pThread->fillFileList( ui->fileListWnd->toPlainText() );
        } else {
            m_pThread->setPath( ui->srcPath->text(), ui->destPath->text(), m_pEncDecObject );

            m_pThread->setDecOnSameFileFlag( ui->decWithNewNameBtn->checkState() == Qt::Checked  );
            m_pThread->setDecAndRenameFileFlag( ui->decWithRenameBtn->checkState() == Qt::Checked  );
        }
    } else if ( ui->cmpBtn->isChecked()  ) {
        // Cmp Mode
        m_pThread->setOp( WorkThread::E_CMP_OP );
        m_pThread->setPath( ui->srcPath->text(), ui->destPath->text(), m_pEncDecObject );

        auto bCmpTargetFiles = (ui->givenFileListBtn->checkState() == Qt::Checked);
        // specify target pairs in file-list-inputbox.    
        //    1.   a pair of files in 2 lines       
        //    2.   multilines pairs seperate with ';'
        if ( !bCmpTargetFiles ) {
            m_pThread->fillFileList("", 1 );
            // compare 2 dirs corresponding paired files
            ui->statusbar->showMessage( QStringLiteral("Compare 2 dirs with corresponding files "), 3500);
        } else {
            auto bUseSemiColonFlag = (ui->cmpSplitBtn->checkState() == Qt::Checked);
            m_pThread->fillFileList( ui->fileListWnd->toPlainText(), (bUseSemiColonFlag ? 2 : 3) );
            ui->statusbar->showMessage( QStringLiteral("Please multiple pairs mode. "), 3500);
        }
    }


    if ( ui->mtThreadBtn->checkState() == Qt::Checked ) {
        //
        // Multi Thread Run
        //
        ui->mtThreadBtn->setEnabled( false );
        // thread is not running now
        m_pThread->start( getThreadPriority() );
    } else { 
        //
        // Single Thread Here
        //
        ui->goBtn->setEnabled( false );
        m_pThread->singleThreadRun();
    }
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







void MainWindow::on_mtThreadBtn_stateChanged(int checkState)
{
    ui->threadPrGrp->setEnabled( checkState != 0 );
}





void MainWindow::on_decWithNewNameBtn_stateChanged(int checkState)
{
    // if checked , disable the input box
    ui->destPath->setEnabled( checkState == 0 );
}




void MainWindow::on_outputFileBtn_clicked()
{
    ui->statusbar->showMessage( QStringLiteral("Travelsal Source Dir and output the scanned files [Only] ."), 5000);
}

// void MainWindow::on_givenFileListBtn_clicked()
// {
//     ui->statusbar->showMessage( QStringLiteral("Apply Operation on the given files listed in [File List] each line ."), 5000);
// }

void MainWindow::on_buildDirOnlyBtn_clicked()
{
    ui->statusbar->showMessage( QStringLiteral("Build same empty directory struct as the source dir [Only] ."), 5000);
}

// void MainWindow::on_cmpSplitBtn_clicked()
// {
//     ui->statusbar->showMessage( QStringLiteral("<path1>/file1;<path2>/file2  [each line makes a pair] ."), 5000);
// }

void MainWindow::on_decWithNewNameBtn_clicked()
{
    ui->statusbar->showMessage( QStringLiteral("Apply Decrypt on the given files and restoring them on the [Same] file ."), 5000);
}

void MainWindow::on_decWithRenameBtn_clicked()
{
    ui->statusbar->showMessage( QStringLiteral("???_enc.WMV  =>  ???.MP4  ."), 5000);
}



void MainWindow::on_givenFileListBtn_stateChanged(int checkState)
{
    auto isChecked = (checkState!=0);
    ui->statusbar->showMessage( QStringLiteral("Apply Operation on the given files listed in [File List] each line ."), 5000);

    ui->srcPath->setEnabled(  !isChecked );
    ui->destPath->setEnabled( !isChecked );

    if ( ui->encBtn->isChecked()  ) {
        if ( isChecked ) {
            ui->outputFileBtn->setCheckState( Qt::Unchecked );
            ui->buildDirOnlyBtn->setCheckState( Qt::Unchecked ); 
        } 
        
        ui->outputFileBtn->setEnabled( !isChecked );
        ui->buildDirOnlyBtn->setEnabled( !isChecked );
        ui->fileListWnd->setEnabled( isChecked );
    } else if ( ui->decBtn->isChecked()  ) {

        ui->fileListWnd->setEnabled( isChecked );
    } else if ( ui->cmpBtn->isChecked() ) {
        ui->cmpSplitBtn->setEnabled( isChecked );
        ui->fileListWnd->setEnabled( isChecked );
        
        if ( isChecked ) {
            ui->fileListWnd->setPlaceholderText("filePath_1;filePath_2\nfilePath_3;filePath_4\n ... \nfilePath_K;filePath_K+1\n");
            ui->statusbar->showMessage( QStringLiteral("<path1>/file1;<path2>/file2  [each line makes a pair] ."), 5000);
        } else {
            ui->fileListWnd->setPlaceholderText("Input 2 file paths on 2 inputboxs : src && dest. ");
            ui->statusbar->showMessage( QStringLiteral("Input 2 file paths on 2 inputboxs : source && destination. "), 5000);
        }
    }

}




/**********************************************************************
  path1/<filename1>;path2/<filename2>
  path3/<filename3>;path4/<filename4>
  path5/<filename5>;path6/<filename6>
      ...
      ...

***********************************************************************/
void MainWindow::on_cmpSplitBtn_stateChanged(int checkState)
{
    auto isChecked = (checkState!=0);
    if ( isChecked ) {
        ui->statusbar->showMessage( QStringLiteral("<path1>/file1;<path2>/file2  [each line makes a pair] ."), 5000);
    } else {
        ui->statusbar->showMessage( QStringLiteral("line1:<path1>/file1<Enter> line2:<path2>/file2 ."), 5000);
    }
}


void MainWindow::on_printFileList(const QString& fileListSortByLayers)
{
    ui->fileListWnd->setPlainText( fileListSortByLayers ); 
}

void MainWindow::on_updateProcessingFileName(const QString& fileMsg)
{
    ui->processedFileEdit->setText( fileMsg );
}



