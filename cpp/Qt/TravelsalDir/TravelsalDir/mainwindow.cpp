#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QItemSelectionModel>
#include <QRegExp>
#include <QMessageBox>
#include <QTableWidget>

static const bool sc_b_USE_ICON = true;

static const QString sc_STATUS_BAR_SHEET_NONE("");
static const QString sc_STATUS_BAR_SHEET_ERROR(R"( color: red; )");
static const QString sc_STATUS_BAR_SHEET_CORRECT(R"( color: green; )");
static const QString sc_STATUS_BAR_SHEET_WARNING(R"( color: #996c33; )");

static const QChar G_SC_OPTION_SEP(';');
static const QString G_SC_WILDCARD_PREFIX("*.");
static const QChar G_SC_CHAR_DOT('.');
static const QString G_SC_STR_DOT(".");


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pFileSystemModel( new QFileSystemModel(this) )
    , m_bPickDirs( false )
    , m_bPickFiles( false )
    , m_ignoredFolderPattern("")
    , m_ignoredFolderPatternList()
    , m_bIgnoredFolderCaseSensitive( false )
    , m_bOnlyVisitMatchedFolder( false )
    , m_ignoredFilePattern("")
    , m_ignoredFilePatternList()
    , m_bIgnoredFileCaseSensitive( false )
    , m_bOnlyVisitMatchedExtFiles( false )
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    , m_timerTotal()
    , m_timerDirs()
    , m_timerFiles()
    , m_bUseMultiThreadMode( true ) // use multi-thread
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    , m_visitedDirCnt( 0ULL )
    , m_visitedFileCnt( 0ULL )
    , m_maxLayer( 0ULL )
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    , m_multiThreadState( 0 )
    , m_pDirVisitThread( nullptr )
    , m_pFileVisitThread( nullptr )
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    , m_extensionMap()
    , m_depthDirs()
    , m_pAllDirs( nullptr )
    , m_generatedTreeNodeList()
    , m_searchMatchedResultNodeList( )
    , m_currentPreviousNextIdx( -1 ) 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    , m_treeFileIcon(":/icons/File.png")
    , m_treeExtIcon(":/icons/Ext.png")
    , m_treeDirIcon(":/icons/Dir.png")
{
    ui->setupUi(this);

    m_extensionMap.clear();

    m_depthDirs.clear();

    m_pAllDirs = new QList<QDir>();

    initUI();

}

MainWindow::~MainWindow()
{
    ui->visitResultTree->clear();

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

    ui->visitResultTree->setHeaderLabels( QStringList{ QString("Name / Tag"), QString("Count / Path") });
    ui->visitResultTree->setColumnWidth(0, 240);
    connect( ui->visitResultTree, SIGNAL( itemSelectionChanged() ), this,  SLOT( on_displayFileContent() ) );

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

    ui->diskTreeView->resizeColumnToContents(0);
    // ui->diskTreeView->resizeColumnToContents(1);

}


//////////////////////////////////////////////////
// Scan Dir
//////////////////////////////////////////////////
void MainWindow::on_scanBtn_clicked()
{
    m_generatedTreeNodeList.clear();
    m_searchMatchedResultNodeList.clear();
    m_currentPreviousNextIdx = -1;

    // Is show folder-tree or not  after travelsal the entire target dir recursively
    m_bPickDirs = ui->folderChk->isChecked();
    m_bPickFiles = ui->fileChk->isChecked();

    m_bOnlyVisitMatchedFolder   = ui->folderMatchChk->isChecked();
    m_bOnlyVisitMatchedExtFiles = ui->fileExtMatchChk->isChecked();

    if ( !m_bPickDirs  && !m_bPickFiles ) {
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
        ui->statusbar->showMessage("[ERROR] : 目录/文件 复选框 至少要选1个", 5000);
        return;
    }

    m_ignoredFolderPattern = ui->ignoreFolderInput->text();
    QString patternDir = m_ignoredFolderPattern.trimmed();
    m_ignoredFolderPatternList.clear();
    if ( !patternDir.isEmpty() ) {
        m_ignoredFolderPatternList = patternDir.split(G_SC_OPTION_SEP);
        for( auto& p : m_ignoredFolderPatternList ) {
            p = p.trimmed();
        }
    }
    // m_bIgnoredFolderCaseSensitive: true  --> case insensitive ( is checked )
    //                                false --> Case   Sensitive ( is unchecked )
    m_bIgnoredFolderCaseSensitive = ui->ignoreFolderChk->isChecked();


    m_ignoredFilePattern = ui->ignoreFileextInput->text();
    QString patternFile = m_ignoredFilePattern.trimmed();
    m_ignoredFilePatternList.clear();
    if ( !patternFile.isEmpty() ) {
        m_ignoredFilePatternList  = patternFile.split(G_SC_OPTION_SEP);
        for( auto& p : m_ignoredFilePatternList ) {
            p = p.trimmed();
        }
    }
    m_bIgnoredFileCaseSensitive = ui->ignoreFileChk->isChecked();




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
    m_pAllDirs->clear();

    m_visitedDirCnt = 0ULL;
    m_visitedFileCnt = 0ULL;
    m_maxLayer = 0ULL;
    m_timerTotal.start();

#ifdef Q_OS_WIN
    auto fullpath = selectedInfo.absoluteFilePath();
    QRegExp re("[a-z]:[\\\\/]?", Qt::CaseInsensitive);
    auto bIsDriveRoot = re.exactMatch(fullpath);
    if ( bIsDriveRoot ) {
        auto sureBtn = QMessageBox::warning(this, "Warning", "Travelsal a [Root-Driver] will take for a long time!\nAre you sure ? ", (QMessageBox::Yes | QMessageBox::No), QMessageBox::No);
        if ( sureBtn == QMessageBox::No ) {
            return;
        }
    }
#else
    auto fullpath = selectedInfo.absoluteFilePath();
    /*

     Root or topLevel dir pattern which is like the following
          /                                      or
          /<word>                                or
          /<word>/                               or
          /<word1> <word2> <word3>               or
          /<word1> <word2> <word3>/

    */
    QRegExp re("/((\\w)*(\\s)*)*/?", Qt::CaseInsensitive);
    auto bIsDriveRoot = re.exactMatch(fullpath);
    if ( bIsDriveRoot ) {
        auto sureBtn = QMessageBox::warning(this, "Warning", "Travelsal a [Root-Dir] will take for a long time!\nAre you sure ? ", (QMessageBox::Yes | QMessageBox::No), QMessageBox::No);
        if ( sureBtn == QMessageBox::No ) {
            return;
        }
    }
#endif

    ui->scanBtn->setEnabled( false );
    ui->clearBtn->setEnabled( false );

    if ( !m_bUseMultiThreadMode  ) {
        qDebug() << "Type == 1 :  Use Single-Thread";

        ui->stopBtn->hide();
        ui->stopBtn->setEnabled(false);


        QDir d( selectedInfo.absoluteFilePath() );
        d.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Hidden );
        m_visitedDirCnt = 1; // take the selected target-root dir into account
        visitDir(d, 0ULL);

        qDebug() << "Elapsed tick = " << m_timerTotal.elapsed() / 1000.0 << " second(s).";

        qDebug() << "DirCnt = "  << m_visitedDirCnt;
        qDebug() << "FileCnt = " << m_visitedFileCnt;
        qDebug() << "MaxLayer = " << m_maxLayer;

        ui->stopBtn->hide();
        ui->stopBtn->setEnabled(false);

        ui->scanBtn->setEnabled( true );
        ui->clearBtn->setEnabled( true );


        fill_ScanResultIntoTreeView( );
    } else {
        qDebug() << "Type == 2 :  Use Multi-Thread";

        ui->stopBtn->show();
        ui->stopBtn->setEnabled(true);

        ui->progressBar->reset();

        QDir d( selectedInfo.absoluteFilePath() );
        d.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden);

        m_depthDirs.clear();
        // CORE CORE CORE : insert root first
        m_visitedDirCnt = 1; // take the selected target-root dir into account
        m_depthDirs.insert(0, QList<QDir>({ QDir(d) }) );

        m_timerDirs.start();

        //
        // Run the background thread
        //
        m_multiThreadState = 1;
        m_pDirVisitThread = new myVisitThread( this );

        m_pDirVisitThread->setType( myVisitThread::E_DIRS );
        // set dir filter ( if a certain dir matched the filter condition ( match with dir.startsWith(pattern) -> true ), !![Do NOT]!! take it into account ( !![DO NOT]!! Add it into the result dir list )  )
        m_pDirVisitThread->setDirIgnoreOption(m_bIgnoredFolderCaseSensitive,  m_ignoredFolderPattern, m_bOnlyVisitMatchedFolder );
        m_pDirVisitThread->setStartDir( d );

        connect(m_pDirVisitThread, &myVisitThread::visitOneDir, this, &MainWindow::onVisitOneDir );
        connect(m_pDirVisitThread, &myVisitThread::selectAllDirs, this, &MainWindow::onVisitAllDirsFinished );
        connect(m_pDirVisitThread, &myVisitThread::finished, m_pDirVisitThread, &QObject::deleteLater );

        m_pDirVisitThread->start();

    }

}

//////////////////////////////////////////////////
// Clear Result
//////////////////////////////////////////////////
void MainWindow::on_clearBtn_clicked()
{
    ui->visitResultTree->clear();

    ui->previousBtn->setEnabled( false );
    ui->nextBtn->setEnabled( false );

    /*
     The Tree Widget will be look like the following struct

       Files
         -- aaa.txt   C:\
         -- bbb.txt   C:\
       Dirs
         -- C:\
    */

    /*
    QTreeWidgetItem *pFileRoot = new QTreeWidgetItem( ui->visitResultTree );
    pFileRoot->setText(0, "Files grouped by extensions ");
    QTreeWidgetItem* pFile1 = new QTreeWidgetItem( pFileRoot );
    pFile1->setText(0, "aaa.txt");
    pFile1->setText(1, "C:\\");

    QTreeWidgetItem* pFile2 = new QTreeWidgetItem( pFileRoot );
    pFile2->setText(0, "bbb.txt");
    pFile2->setText(1, "C:\\");


    QTreeWidgetItem *pDirRoot = new QTreeWidgetItem( ui->visitResultTree );
    pDirRoot->setText(0, "Dirs");
    QTreeWidgetItem* pDir1 = new QTreeWidgetItem( pDirRoot );
    pDir1->setText(0, "C:\\");
    */


/*


    // QFileInfo f("C:\\xxx\\yyy\\main.txt");
    QFileInfo f("C:/xxx/yyy/main.txt.backup");

    qDebug() << "f.dir = |" << f.dir() << "|";                           // dir = C:/xxx/yyy
    qDebug() << "f.fileName = |" << f.fileName() << "|";                 // fileName = main.txt.backup
    qDebug() << "f.filePath = |" << f.filePath() << "|";                 // filePath = C:/xxx/yyy/main.txt.backup
    qDebug() << "f.path = |" << f.path() << "|";                         // path = C:/xxx/yyy
    qDebug() << "f.baseName = |" << f.baseName() << "|";                 // baseName = main
    qDebug() << "f.completeBaseName = |" << f.completeBaseName() << "|"; // completeBaseName = main.txt
    qDebug() << "f.suffix = |" << f.suffix() << "|";                     // suffix = backup
    qDebug() << "f.completeSuffix = |" << f.completeSuffix() << "|";     // completeSuffix = txt.backup


*/

/*
    QRegExp re1("(hello|world)+", Qt::CaseInsensitive);

    QString s1("hellohello");
    QString s2("Worldworld");
    QString s3("helloworld");
    QString s4("Worldhello");

    qDebug() << "re1 exactMatch s1 ? = " << re1.exactMatch(s1);
    qDebug() << "re1 exactMatch s2 ? = " << re1.exactMatch(s2);
    qDebug() << "re1 exactMatch s3 ? = " << re1.exactMatch(s3);
    qDebug() << "re1 exactMatch s4 ? = " << re1.exactMatch(s4);

    // qDebug() << "re2 exactMatch s1 ? = " << re2.exactMatch(s1);
    // qDebug() << "re2 exactMatch s2 ? = " << re2.exactMatch(s2);
*/


// #ifdef Q_OS_WIN
//     QString testAry[] = {
//         QString("/"),
//         QString("/usr"),
//         QString("/usr/"),
//
//         QString("/a b"),
//         QString("/a b/"),
//
//         QString("/a b c"),
//         QString("/a b c/")
//     };
//
//
//     QRegExp re("/((\\w)*(\\s)*)*/?", Qt::CaseInsensitive);
//     for( const auto& s : testAry ) {
//         auto b = re.exactMatch(s);
//         qDebug() << s << " matched ? " << b;
//     }
// #else
// #endif

}


//////////////////////////////////////////////////
// Stop Scaning
//////////////////////////////////////////////////
void MainWindow::on_stopBtn_clicked()
{
    if ( m_multiThreadState == 1 ) {
        if ( m_pDirVisitThread !=nullptr ) {
            m_pDirVisitThread->setTerminateFlag();

            // m_pDirVisitThread->terminate();
            // if ( !m_pDirVisitThread->wait(2000) ) {
            //     m_pDirVisitThread->quit();
            // }

            ui->scanBtn->setEnabled( true );
            ui->clearBtn->setEnabled( true );

            ui->stopBtn->setEnabled( false );
            ui->stopBtn->hide();

            ui->statusbar->clearMessage();
            ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_WARNING  );
            ui->statusbar->showMessage( QString("[WARNING] User Cancled ") , 3000 );

        }

        m_multiThreadState = 0;
    } else if ( m_multiThreadState == 2 ) {
        if ( m_pFileVisitThread !=nullptr ) {
            m_pFileVisitThread->setTerminateFlag();

            //  m_pFileVisitThread->terminate();
            //  if ( !m_pFileVisitThread->wait(2000) ) {
            //      m_pFileVisitThread->quit();
            //  }

            // m_pFileVisitThread->exit(0);

            ui->scanBtn->setEnabled( true );
            ui->clearBtn->setEnabled( true );

            ui->stopBtn->setEnabled( false );
            ui->stopBtn->hide();

            ui->statusbar->clearMessage();
            ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_WARNING  );
            ui->statusbar->showMessage( QString("[WARNING] User Cancled ") , 3000 );
        }

        m_multiThreadState = 0;
    }



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
    focusPreviousBtnMatched();
}


//////////////////////////////////////////////////
// Next Matched
//////////////////////////////////////////////////
void MainWindow::on_nextBtn_clicked()
{
    focusNextBtnMatched();
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
    // QModelIndex QFileSystemModel::index(const QString &path, int column = 0)
    auto absPath = ui->pickFolderInput->text().trimmed();
    if ( absPath.isEmpty() ) {
        return;
    }

    QFileInfo d(absPath);
    if ( d.isDir() ) {
        QModelIndex pickedByInputDirModel = m_pFileSystemModel->index(absPath, 0);
        if ( pickedByInputDirModel.isValid() ) {
            QItemSelectionModel* selModel = ui->diskTreeView->selectionModel();
            if ( selModel != nullptr ) {
                selModel->clear();
                selModel->select( pickedByInputDirModel, QItemSelectionModel::SelectCurrent);
            }

            ui->diskTreeView->scrollTo( pickedByInputDirModel, QAbstractItemView::EnsureVisible);
            ui->diskTreeView->setExpanded(pickedByInputDirModel, true);
        }
    }

}



void MainWindow::visitDir(const QDir& toBeTravelsaled, unsigned long long layer)
{
    if ( layer > m_maxLayer ) {
        m_maxLayer = layer;
    }

    if ( layer == 0 ) {
        m_pAllDirs->push_back( toBeTravelsaled );
    }

    // qDebug() << "dir = " << toBeTravelsaled;
    auto fInfoEntryList = toBeTravelsaled.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Hidden );
    for( auto i = 0; i < fInfoEntryList.size(); ++i ) {
        auto eachfInfo = fInfoEntryList.at(i);
        if ( eachfInfo.isDir() ) {
            QDir dirObj( eachfInfo.absoluteFilePath() );

            auto isMatched = false;
            for( auto i = 0; i < m_ignoredFolderPatternList.size(); ++i )
            {
                auto pattern = m_ignoredFolderPatternList.at(i);
                if ( !pattern.isEmpty() ) {
                    isMatched = ( eachfInfo.fileName().startsWith(pattern, (m_bIgnoredFolderCaseSensitive ? Qt::CaseInsensitive : Qt::CaseSensitive)) );
                    if ( isMatched ) {
                        break;
                    }
                }
            }

            auto bNeedVisit = false;
            if ( m_bOnlyVisitMatchedFolder ) {
                bNeedVisit = isMatched;
            } else {
                bNeedVisit = !isMatched;
            }

            if ( bNeedVisit ) {
                ++m_visitedDirCnt;
                m_pAllDirs->push_back( dirObj );
                visitDir(dirObj, layer+1 );
            } else {
                // skip this dir , maybe TODO something
            }
        } else {
            if ( m_bPickFiles ) {
                // it is a file
                auto fullpath = eachfInfo.absoluteFilePath();
                auto completeSuffixExt = eachfInfo.completeSuffix();
                auto suffixExt = eachfInfo.suffix();

                auto isMatchedFile = false;
                for( auto i = 0; i <  m_ignoredFilePatternList.size(); ++i )
                {
                    auto pattern = m_ignoredFilePatternList.at(i);

                    if ( !pattern.isEmpty() ) {
                        // start wit  "*."
                        if ( pattern.startsWith(G_SC_WILDCARD_PREFIX) ) {
                            auto ext = pattern.mid(2);
                            // such as   *.config.backup
                            QString realSuffix;
                            if ( ext.contains( G_SC_CHAR_DOT ) ) { // the suffix is more than one '.'
                                realSuffix = completeSuffixExt;
                            } else {
                                realSuffix = suffixExt;
                            }

                            isMatchedFile = (realSuffix.compare(ext, (m_bIgnoredFileCaseSensitive ? Qt::CaseInsensitive : Qt::CaseSensitive)) == 0);
                        } else {
                            // use startsWith strategy to compare with
                            isMatchedFile = ( eachfInfo.fileName().startsWith(pattern, (m_bIgnoredFileCaseSensitive  ? Qt::CaseInsensitive : Qt::CaseSensitive)) );
                        }

                        if ( isMatchedFile ) {
                            break;
                        }
                    }
                }

                auto bNeedVisitFile = false;
                if ( m_bOnlyVisitMatchedExtFiles ) {
                    bNeedVisitFile = isMatchedFile;
                } else {
                    bNeedVisitFile = !isMatchedFile;
                }

                if ( bNeedVisitFile ) {
                    ++m_visitedFileCnt;
                    // such as  "Makefile"
                    if ( completeSuffixExt.isEmpty() && suffixExt.isEmpty() ) {
                        // such as  "/usr/hello." ( [Pay Attention] Please :  there has a '.' after the word "hello"  ), it has neither completeSuffix() nor suffix()
                        if ( !fullpath.contains( G_SC_CHAR_DOT  ) ) {
                            m_extensionMap["<No-Suffix>"].push_back( eachfInfo );
                        } else {
                            m_extensionMap["<Suffix-IS-Empty>"].push_back( eachfInfo );
                        }
                    } else {
                        if ( completeSuffixExt != suffixExt ) {
                            m_extensionMap[G_SC_STR_DOT + completeSuffixExt].push_back( eachfInfo );
                        } else {
                            m_extensionMap[G_SC_STR_DOT  + suffixExt].push_back( eachfInfo ) ;
                        }
                    }
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
    if ( m_multiThreadState == 0 ) {
        // User canceled
        return;
    }

    qDebug() << "Elapsed Dirs tick = " << m_timerDirs.elapsed() / 1000.0 << " second(s).";
    // m_timerDirs.start();

    m_pAllDirs->clear();
    for( auto it = m_depthDirs.begin(); it!=m_depthDirs.end(); ++it ) {
        m_pAllDirs->append( it.value() );
    }

    m_timerFiles.start();
    if ( !m_bPickFiles ) {
        onVisitAllFileFinished();
        return;
    }

    m_multiThreadState = 2;
    m_pFileVisitThread = new myVisitThread(this);
    m_pFileVisitThread->setType( myVisitThread::E_FILES );

    // set file filter ( if a certain file matched the filter condition, !![Do NOT]!! take it into account ( do not add it into the result file list )  )
    m_pFileVisitThread->setFileIgnoreOption( m_bIgnoredFileCaseSensitive, m_ignoredFilePattern, m_bOnlyVisitMatchedExtFiles );

    m_pFileVisitThread->setVisitedDirs( m_pAllDirs );
    connect(m_pFileVisitThread, &myVisitThread::visitOneFile, this, &MainWindow::onVisitOneFile );
    connect(m_pFileVisitThread, &myVisitThread::visitOneDirAllFiles, this, &MainWindow::onVisitOneDirAllFiles );
    connect(m_pFileVisitThread, &myVisitThread::visitAllDone, this, &MainWindow::onVisitAllFileFinished );
    connect(m_pFileVisitThread, &myVisitThread::finished, m_pFileVisitThread, &QObject::deleteLater );

    m_pFileVisitThread->start();

}

void MainWindow::onVisitOneFile(int idx, const QFileInfo& finfo)
{
    Q_UNUSED(idx)
    ++m_visitedFileCnt;

    auto fullpath = finfo.fileName();
    auto completeSuffixExt = finfo.completeSuffix();
    auto suffixExt = finfo.suffix();

    if ( completeSuffixExt.isEmpty() && suffixExt.isEmpty() ) {
        // such as  "/usr/hello." ( [Pay Attention] Please :  there has a '.' after the word "hello"  ), it has neither completeSuffix() nor suffix()
        if ( !fullpath.contains( G_SC_CHAR_DOT  ) ) {
            m_extensionMap["<No-Suffix>"].push_back( finfo );
        } else {
            m_extensionMap["<Suffix-IS-Empty>"].push_back( finfo );
        }
    } else {
        if ( completeSuffixExt != suffixExt ) {
            m_extensionMap[G_SC_STR_DOT + completeSuffixExt].push_back( finfo );
        } else {
            m_extensionMap[G_SC_STR_DOT  + suffixExt].push_back( finfo ) ;
        }
    }

    ui->statusbar->clearMessage();
    ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_NONE );
    // qDebug() << "pick file , cnt = " << m_visitedFileCnt;
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

    if ( !m_bPickFiles ) {
        ui->statusbar->showMessage("[INFO] : Visit All-Files skipped, [DONE] ", 3000);
    } else {
        ui->statusbar->showMessage("[INFO] : Visit All-Files Finished ", 3000);
    }


    fill_ScanResultIntoTreeView();
}


void MainWindow::onVisitOneDirAllFiles(unsigned long long finishedCnt)
{
    if ( finishedCnt <= m_visitedDirCnt ) {
        ui->progressBar->setValue( static_cast<int>( finishedCnt * 1.0 / m_visitedDirCnt * 100.0) );
    } else {
        ui->progressBar->setValue( 100 );
    }
}


void MainWindow::fill_ScanResultIntoTreeView()
{
    m_generatedTreeNodeList.clear();
    m_searchMatchedResultNodeList.clear();
    m_currentPreviousNextIdx = -1;

    m_multiThreadState = 0;

    ui->visitResultTree->clear();

    QTreeWidgetItem *pFileRoot = new QTreeWidgetItem( ui->visitResultTree );
    pFileRoot->setText(0, "Files");
    pFileRoot->setText(1, QString("%1 file(s) of %2 ext-kinds").arg( m_visitedFileCnt ).arg( m_extensionMap.size() ) );
    if ( sc_b_USE_ICON ) { pFileRoot->setIcon(0, m_treeFileIcon ); }

    if ( m_bPickFiles ) {
        auto idx = 0;
        for ( auto it = m_extensionMap.begin(); it!=m_extensionMap.end(); ++it, ++idx ) {
            QTreeWidgetItem *pFileExtTreeRoot = new QTreeWidgetItem( pFileRoot );
            pFileExtTreeRoot->setText(0, QString("#%1 %2").arg(idx+1).arg( it.key() ) );
            if ( sc_b_USE_ICON ) { pFileExtTreeRoot->setIcon(0, m_treeExtIcon  ); }
            pFileExtTreeRoot->setText(1, QString("%1").arg( it.value().size() ) );

            for( auto fileit = it.value().begin(); fileit!=it.value().end(); ++fileit ) {
                QTreeWidgetItem *pFile = new QTreeWidgetItem(pFileExtTreeRoot );
                auto fileName = fileit->fileName();
                m_generatedTreeNodeList.push_back( pFile );
                pFile->setText(0, fileName );
                pFile->setText(1, QString("%1").arg( fileit->absoluteFilePath() ) );
                if ( sc_b_USE_ICON ) { pFile->setIcon(0, m_treeFileIcon  ); }
                pFile->setFlags(pFile->flags() | Qt::ItemIsEditable );
                // QVariant(1) -> file   |  QVariant(2) -> dir
                pFile->setData(0,  Qt::UserRole, QVariant(1) );
            }
        }
    }


    QTreeWidgetItem *pDirRoot = new QTreeWidgetItem( ui->visitResultTree );
    pDirRoot->setText(0, "Dirs");
    pDirRoot->setText(1, QString("%1 count of %2 types").arg(m_visitedDirCnt).arg(m_bPickDirs ?  m_pAllDirs->size() : 0 ) );
    if ( sc_b_USE_ICON ) { pDirRoot->setIcon(0, m_treeDirIcon  ); }
    if ( m_bPickDirs ) {
        if ( m_pAllDirs != nullptr && !m_pAllDirs->isEmpty() ) {
            QMap<QString, QList<QDir> > groups;

            for ( auto it = m_pAllDirs->begin(); it!=m_pAllDirs->end(); ++it ) {
                groups[ it->dirName()  ].push_back( *it );
            }

            auto idx = 0;
            for( auto it = groups.begin(); it!=groups.end(); ++it, ++idx ) {
                QTreeWidgetItem *pDirType = new QTreeWidgetItem( pDirRoot );
                pDirType->setText(0, QString("#%1 %2").arg(idx+1).arg( it.key() ) );
                pDirType->setText(1, QString("%1 with same name").arg(it.value().size()) );
                if ( sc_b_USE_ICON ) { pDirType->setIcon(0, m_treeDirIcon  ); }

                for( auto it2 = it.value().begin(); it2!=it.value().end(); ++it2 ) {
                    QTreeWidgetItem *pDirObj = new QTreeWidgetItem( pDirType );

                    m_generatedTreeNodeList.push_back( pDirObj );

                    pDirObj->setText(0, QString("%1").arg( it2->absolutePath() ) );
                    pDirObj->setFlags(pDirObj->flags() | Qt::ItemIsEditable );

                    // QVariant(1) -> file   |  QVariant(2) -> dir
                    pDirObj->setData(0, Qt::UserRole, QVariant(2) );

                    if ( sc_b_USE_ICON ) { pDirObj->setIcon(0, m_treeDirIcon  ); }
                }
            }
        }
    }

    ui->visitResultTree->setColumnWidth(0, 240);

}






void MainWindow::on_resultFileSearhingInput_returnPressed()
{
    m_searchMatchedResultNodeList.clear();
    m_currentPreviousNextIdx = -1;
    ui->previousBtn->setEnabled(false);
    ui->nextBtn->setEnabled(false);
    if ( m_multiThreadState != 0 ) {
        return;
    }

    auto strSearchWhat = ui->resultFileSearhingInput->text().trimmed();
    if ( strSearchWhat.isEmpty() ) {
        return;
    }


    auto bIsIgnoreSearchResultCaseSensitive = ui->prevNextIgnoreChk->isChecked();
    for ( auto item : m_generatedTreeNodeList ) {
        if ( item!=nullptr ) {
            if ( item->text(0).contains(strSearchWhat, bIsIgnoreSearchResultCaseSensitive ? Qt::CaseInsensitive : Qt::CaseSensitive) ) {
                m_searchMatchedResultNodeList.push_back( item );
            }
        }
    }

    if ( m_searchMatchedResultNodeList.empty() ) {
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR  );
        ui->statusbar->showMessage( QString("[ERROR] Oops ... No item matched keyword !") , 3000 );

        ui->previousBtn->setEnabled(false);
        ui->nextBtn->setEnabled(false);
        m_currentPreviousNextIdx = -1;
    } else {
        ui->previousBtn->setEnabled( true );
        ui->nextBtn->setEnabled( true );

        m_currentPreviousNextIdx = 0;
        focusSearchMatched();
    }

}

void MainWindow::focusSearchMatched()
{
    auto sz = m_searchMatchedResultNodeList.size();
    if ( sz == 0  || m_currentPreviousNextIdx == -1 ) {
        return;
    }

    if ( m_currentPreviousNextIdx>=0 && m_currentPreviousNextIdx < sz ) {
        auto selItem = m_searchMatchedResultNodeList.at(m_currentPreviousNextIdx);
        if ( selItem!=nullptr ) {
            ui->visitResultTree->setCurrentItem(selItem, 0, QItemSelectionModel::ClearAndSelect );
            ui->visitResultTree->scrollToItem( selItem );

        }
    }
}

void MainWindow::focusNextBtnMatched()
{
    auto isLoop = ui->loopChk->isChecked();
    auto sz = m_searchMatchedResultNodeList.size();
    if ( sz == 0  || m_currentPreviousNextIdx == -1 ) {
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_WARNING  );
        ui->statusbar->showMessage( QString("[WARNING] No Item(s) matched keyword!") , 3000 );
        return;
    }

    ++m_currentPreviousNextIdx;
    if ( isLoop ) {
        m_currentPreviousNextIdx = (m_currentPreviousNextIdx + sz) % sz;
    }

    if ( m_currentPreviousNextIdx >= sz ) {
        m_currentPreviousNextIdx = sz-1;

        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_WARNING  );
        ui->statusbar->showMessage( QString("[WARNING] Reached <Last> matched item ") , 3000 );
        return;
    }

    focusSearchMatched();
}

void MainWindow::focusPreviousBtnMatched()
{
    auto isLoop = ui->loopChk->isChecked();
    auto sz = m_searchMatchedResultNodeList.size();
    if ( sz == 0  || m_currentPreviousNextIdx == -1 ) {
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_WARNING  );
        ui->statusbar->showMessage( QString("[WARNING] No Item(s) matched keyword!") , 3000 );
        return;
    }

    --m_currentPreviousNextIdx;
    if ( isLoop ) {
        m_currentPreviousNextIdx = (m_currentPreviousNextIdx + sz) % sz;
    }

    if ( m_currentPreviousNextIdx < 0 ) {
        m_currentPreviousNextIdx = 0;

        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_WARNING  );
        ui->statusbar->showMessage( QString("[WARNING] Reached <First> matched item !") , 3000 );
        return;
    }
    focusSearchMatched();
}


void MainWindow::on_displayFileContent()
{
    auto items = ui->visitResultTree->selectedItems();
    if ( items.size() != 1 ) {
        return;
    }

    auto item = items.at(0);
    if ( item!=nullptr ) {
        auto vdata = item->data( 0, Qt::UserRole);
        auto isValid = false;
        auto val = vdata.toInt(&isValid);
        if ( isValid ) {
            if ( val == 1 ) {
                // File
                auto dirOrFile_Path = item->text(1);
                ui->processingFolderPath->setText( dirOrFile_Path );
                QFileInfo info( dirOrFile_Path );
                if ( info.isFile() ) {
                    QFile file( dirOrFile_Path );
                    if ( file.open( QIODevice::ReadOnly | QIODevice::ExistingOnly) ) {
                        auto byteArray = file.readAll();
                        ui->fileContentView->setPlainText( QString(byteArray) );
                    } else {
                        ui->statusbar->clearMessage();
                        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
                        ui->statusbar->showMessage("[ERROR] : 打开文件失败", 5000);
                    }
                    file.close();
                }
            }
        }

    }


}


