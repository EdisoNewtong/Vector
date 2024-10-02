#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QItemSelectionModel>
#include "myVisitThread.h"

static const QString sc_STATUS_BAR_SHEET_NONE("");
static const QString sc_STATUS_BAR_SHEET_ERROR(R"( color: red; )");
static const QString sc_STATUS_BAR_SHEET_CORRECT(R"( color: green; )");


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pFileSystemModel( new QFileSystemModel(this) )
    , m_timerTotal()
    , m_timerDirs()
    , m_timerFiles()
    , m_bUseMultiThreadMode( true ) // use multi-thread
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    , m_visitedDirCnt( 0ULL )
    , m_visitedFileCnt( 0ULL )
    , m_maxLayer( 0ULL )
    , m_extensionMap()
    , m_depthDirs()
    , m_pAllDirs( nullptr )
{
    ui->setupUi(this);

    m_extensionMap.clear();
    m_depthDirs.clear();

    m_pAllDirs = new QList<QDir>();

    initUI();

}

MainWindow::~MainWindow()
{
    delete ui;

    if ( m_pAllDirs!=nullptr ) {
        m_pAllDirs->clear();
        delete m_pAllDirs;
    }
}


void MainWindow::initUI()
{
    // ui->fileContentView->appendHtml(QString(R"(<span style="font-size:100px; color:red;">aaa</span><span style="font-size:50px; color:green;">bbb</span><span style="font-size:25px; color:blue;">ccc</span>)"));

    ui->stopBtn->hide();
    ui->stopBtn->setEnabled(false);

    ui->previousBtn->setEnabled(false);
    ui->nextBtn->setEnabled(false);

    if ( m_bUseMultiThreadMode ) {
        ui->actionMultiThread->setChecked( true );
        ui->actionSingleThread->setChecked( false );
    } else {
        ui->actionMultiThread->setChecked( false );
        ui->actionSingleThread->setChecked( true );
    }


    refreshFileSystemModel( false );
}


void MainWindow::refreshFileSystemModel(bool needDeletePrevious)
{
    if ( needDeletePrevious ) {
        QItemSelectionModel* selmodel = ui->diskTreeView->selectionModel();
        if ( selmodel!=nullptr ) {
            delete selmodel;
        }

        ui->diskTreeView->setModel( nullptr );
    }


    m_pFileSystemModel->setRootPath( QDir::currentPath() );
    // m_pFileSystemModel->setRootPath( QDir::rootPath() );
    m_pFileSystemModel->setFilter( QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden );
    ui->diskTreeView->setModel( m_pFileSystemModel );
}


//////////////////////////////////////////////////
// Scan Dir
//////////////////////////////////////////////////
void MainWindow::on_scanBtn_clicked()
{
    if ( !ui->folderChk->isChecked()  && !ui->fileChk->isChecked() ) {
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
        ui->statusbar->showMessage("[ERROR] : 目录/文件 复选框 至少要选1个", 5000);
        return;
    }


    QFileInfo selectedInfo;
    QItemSelectionModel* selModel = ui->diskTreeView->selectionModel();
    if ( selModel!=nullptr ) {
        auto selections = selModel->selectedIndexes();
        auto sz = selections.size();

        if ( sz == 0 ) {
            ui->statusbar->clearMessage();
            ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
            ui->statusbar->showMessage("[ERROR] : 请至少选择1个目录 或 盘符", 5000);
            return;
        } else if ( sz > 1 ) {
            ui->statusbar->clearMessage();
            ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
            ui->statusbar->showMessage("[ERROR] : 请 至多 选择1个目录 或 盘符", 5000);
            return;
        }

        // sz == 1
        selectedInfo = m_pFileSystemModel->fileInfo( selections.at(0) );
    }

    m_extensionMap.clear();
    m_visitedDirCnt = 0ULL;
    m_visitedFileCnt = 0ULL;
    m_maxLayer = 0ULL;
    m_timerTotal.restart();

    ui->scanBtn->setEnabled( false );
    ui->clearBtn->setEnabled( false );

    if ( !m_bUseMultiThreadMode  ) {
        ui->stopBtn->hide();
        ui->stopBtn->setEnabled(false);

        qDebug() << "Type == 1 :  Use Single-Thread";

        QDir d( selectedInfo.absoluteFilePath() );
        d.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Hidden );
        visitDir(d, 0ULL);

        qDebug() << "Elapsed tick = " << m_timerTotal.elapsed() / 1000.0 << " second(s).";

        qDebug() << "DirCnt = "  << m_visitedDirCnt;
        qDebug() << "FileCnt = " << m_visitedFileCnt;
        qDebug() << "MaxLayer = " << m_maxLayer;

        ui->stopBtn->hide();
        ui->stopBtn->setEnabled(false);

        ui->scanBtn->setEnabled( true );
        ui->clearBtn->setEnabled( true );

        // auto idx = 0;
        // for( auto it = m_extensionMap.begin(); it!=m_extensionMap.end(); ++it , ++idx ) {
        //     qDebug() << (idx+1) << ". " << it.key() << " ->  CNT = " << it.value();
        // }
    } else {
        ui->stopBtn->show();
        ui->stopBtn->setEnabled(true);

        ui->progressBar->reset( );


        qDebug() << "Type == 2 :  Use Multi-Thread";

        QDir d( selectedInfo.absoluteFilePath() );
        d.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden);

        m_depthDirs.clear();
        // CORE CORE CORE : insert root first
        m_depthDirs.insert(0, QList<QDir>({ QDir(d) }) );

        m_timerDirs.restart();

        //
        // Run the background thread
        //
        myVisitThread* pRunningThread = new myVisitThread(this);
        pRunningThread->setType( myVisitThread::E_DIRS );
        pRunningThread->setStartDir( d );
        connect(pRunningThread, &myVisitThread::visitOneDir, this, &MainWindow::onVisitOneDir );
        connect(pRunningThread, &myVisitThread::selectAllDirs, this, &MainWindow::onVisitAllDirsFinished );
        connect(pRunningThread, &myVisitThread::finished, pRunningThread, &QObject::deleteLater );

        pRunningThread->start();

    }

}

//////////////////////////////////////////////////
// Clear Result
//////////////////////////////////////////////////
void MainWindow::on_clearBtn_clicked()
{

}


//////////////////////////////////////////////////
// Stop Scaning
//////////////////////////////////////////////////
void MainWindow::on_stopBtn_clicked()
{

}


//////////////////////////////////////////////////
// Flip  "/" <-> "\"
//
//     c:\aaa\bbb    <==>   c:/aaa/bbb
//
//////////////////////////////////////////////////
void MainWindow::on_slashFlipBtn_clicked()
{

}



//////////////////////////////////////////////////
// Previous Matched
//////////////////////////////////////////////////
void MainWindow::on_previousBtn_clicked()
{

}


//////////////////////////////////////////////////
// Next Matched
//////////////////////////////////////////////////
void MainWindow::on_nextBtn_clicked()
{

}



//////////////////////////////////////////////////
// Refresh Disk/Dirs
//////////////////////////////////////////////////
void MainWindow::on_refreshDirBtn_clicked()
{
    refreshFileSystemModel(true);
}


//////////////////////////////////////////////////
// Select folder by input
//////////////////////////////////////////////////
void MainWindow::on_pickFolderInput_returnPressed()
{

}



void MainWindow::visitDir(const QDir& toBeTravelsaled, unsigned long long layer)
{
    if ( layer > m_maxLayer ) {
        m_maxLayer = layer;
    }
    // ++m_visitedDirCnt;

    // qDebug() << "dir = " << toBeTravelsaled;
    auto entryList = toBeTravelsaled.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Hidden );
    for( auto i = 0; i < entryList.size(); ++i ) {
        auto eachfInfo = entryList.at(i);
        if ( eachfInfo.isDir() ) {
            ++m_visitedDirCnt;
            QDir dirObj( eachfInfo.absoluteFilePath() );
            visitDir(dirObj, layer+1 );
        } else {
            ++m_visitedFileCnt;
            // it is a file
            auto completeSuffixExt = eachfInfo.completeSuffix();
            auto suffixExt = eachfInfo.suffix();

            // such as  "Makefile"
            if ( completeSuffixExt.isEmpty() && suffixExt.isEmpty() ) {
                ++m_extensionMap["<No-Suffix>"];
            } else {
                if ( completeSuffixExt != suffixExt ) {
                    ++m_extensionMap[QString(".") + completeSuffixExt];
                } else {
                    ++m_extensionMap[QString(".") + suffixExt];
                }
            }
        }
    }
}



void MainWindow::on_actionMultiThread_triggered()
{
    m_bUseMultiThreadMode = true;

    ui->actionMultiThread->setChecked( true );
    ui->actionSingleThread->setChecked( false );
}

void MainWindow::on_actionSingleThread_triggered()
{
    m_bUseMultiThreadMode = false;

    ui->actionSingleThread->setChecked( true );
    ui->actionMultiThread->setChecked( false );
}

void MainWindow::onVisitOneDir(int idx, const QFileInfo& finfo,unsigned long long layer)
{
    Q_UNUSED(idx)
    if ( layer > m_maxLayer  ) {
        m_maxLayer = layer;
    }

    auto it = m_depthDirs.find(layer);
    auto path = finfo.absoluteFilePath();
    if ( it == m_depthDirs.end() ) {
        m_depthDirs.insert( layer, QList<QDir>({ QDir(path) }) );
    } else {
        it.value().push_back( QDir(path)  );
    }
    ui->processingFolderPath->setText( path );
    ++m_visitedDirCnt;
}

void MainWindow::onVisitAllDirsFinished()
{
    qDebug() << "Elapsed Dirs tick = " << m_timerDirs.elapsed() / 1000.0 << " second(s).";
    // m_timerDirs.start();

    m_pAllDirs->clear();
    for( auto it = m_depthDirs.begin(); it!=m_depthDirs.end(); ++it ) {
        m_pAllDirs->append( it.value() );
    }

    m_timerFiles.start();

    myVisitThread* pRunningThread = new myVisitThread(this);
    pRunningThread->setType( myVisitThread::E_FILES );
    pRunningThread->setVisitedDirs( m_pAllDirs );
    connect(pRunningThread, &myVisitThread::visitOneFile, this, &MainWindow::onVisitOneFile );
    connect(pRunningThread, &myVisitThread::visitOneDirAllFiles, this, &MainWindow::onVisitOneDirAllFiles );
    connect(pRunningThread, &myVisitThread::visitAllDone, this, &MainWindow::onVisitAllFileFinished );
    connect(pRunningThread, &myVisitThread::finished, pRunningThread, &QObject::deleteLater );

    pRunningThread->start();

}

void MainWindow::onVisitOneFile(int idx, const QFileInfo& finfo)
{
    Q_UNUSED(idx)
    ++m_visitedFileCnt;

    ui->statusbar->clearMessage();
    ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_NONE );
    qDebug() << "pick file , cnt = " << m_visitedFileCnt;
    ui->statusbar->showMessage( QString("%1. %2").arg(m_visitedFileCnt).arg(finfo.absoluteFilePath() ), 100 );

}

void MainWindow::onVisitAllFileFinished()
{
    qDebug() << "Elapsed Files tick = " << m_timerFiles.elapsed() / 1000.0 << " second(s).";
    qDebug() << "Elapsed Total tick = " << m_timerTotal.elapsed() / 1000.0 << " second(s).";
    qDebug() << "DirCnt = "  << m_visitedDirCnt;
    qDebug() << "FileCnt = " << m_visitedFileCnt;

    ui->clearBtn->setEnabled( true );
    ui->scanBtn->setEnabled( true );

    ui->stopBtn->hide();
    ui->stopBtn->setEnabled(false);

    ui->statusbar->clearMessage();
    ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_CORRECT );
    ui->statusbar->showMessage("[INFO] : Visit Finished ", 3000);

}


void MainWindow::onVisitOneDirAllFiles(unsigned long long finishedCnt)
{
    if ( finishedCnt <= m_visitedDirCnt ) {
        ui->progressBar->setValue( static_cast<int>( finishedCnt * 1.0 / m_visitedDirCnt * 100.0) );
    } else {
        ui->progressBar->setValue( 100 );
    }
}


