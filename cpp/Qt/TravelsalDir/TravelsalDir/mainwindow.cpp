#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QUrl>
#include <QImage>
#include <QImageReader>
#include <QPixmap>
#include <QFile>
#include <QItemSelectionModel>
#include <QRegExp>
#include <QMessageBox>
#include <QTableWidget>
#include <QMap>

static const bool sc_b_USE_ICON = true;

static const QString sc_STATUS_BAR_SHEET_NONE("");
static const QString sc_STATUS_BAR_SHEET_ERROR(R"( color: red; )");
static const QString sc_STATUS_BAR_SHEET_CORRECT(R"( color: green; )");
static const QString sc_STATUS_BAR_SHEET_WARNING(R"( color: #996c33; )");

static const QChar G_SC_OPTION_SEP(';');
static const QString G_SC_WILDCARD_PREFIX("*.");
static const QChar G_SC_CHAR_DOT('.');
static const QString G_SC_STR_DOT(".");
// static const QChar G_SC_CHAR_LESS_THAN('<');

static const QString G_SC_NO_SUFFIX       = "<No-Suffix>";
static const QString G_SC_SUFFIX_IS_EMPTY = "<Suffix-IS-Empty>";
static const QString G_SC_MULTI_ROOT      = "<Mt-Root>";


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
    , m_bSeperateMultiExtensionFlag( true )
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    , m_timerTotal()
    , m_timerDirs()
    , m_timerFiles()
    , m_bUseMultiThreadMode( true ) // use multi-thread
    , m_bSkipSymbol_link_dirFlag( true ) // use multi-thread
    , m_bSkipSymbol_link_fileFlag( true ) // use multi-thread
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
    , m_extTypeGroupCnt( 0 )
    , m_extTypeGroupPartCnt( 0 )
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    , m_treeFileIcon( new QIcon(":/icons/File.png") )
    , m_treeExtIcon(  new QIcon(":/icons/Ext.png")  )
    , m_treeDirIcon(  new QIcon(":/icons/Dir.png")  )
    , m_treeMtExtIcon(  new QIcon(":/icons/Mt-Ext.png")  )
{
    ui->setupUi(this);

    m_extensionMap.clear();

    m_depthDirs.clear();

    m_pAllDirs = new QVector<QDir>();

    initUI();

}

MainWindow::~MainWindow()
{
    ui->visitResultTree->clear();

    delete ui;

    delete m_treeFileIcon; m_treeFileIcon = nullptr;
    delete m_treeExtIcon;  m_treeExtIcon = nullptr;
    delete m_treeDirIcon;  m_treeDirIcon = nullptr;

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
    ui->actionSkipDir->setChecked( m_bSkipSymbol_link_dirFlag );
    ui->actionSkipFile->setChecked( m_bSkipSymbol_link_fileFlag );


    ui->visitResultTree->setHeaderLabels( QStringList{ QString("Name / Tag"), QString("Count / Path"), QString("Group Details") });
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

    m_extTypeGroupCnt = 0;
    m_extTypeGroupPartCnt = 0;


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
    ui->visitResultTree->clear();
    ui->visitResultTree->setEnabled( false );

    if ( !m_bUseMultiThreadMode  ) {
        qDebug() << "Type == 1 :  Use Single-Thread";

        ui->stopBtn->hide();
        ui->stopBtn->setEnabled(false);


        QDir d( selectedInfo.absoluteFilePath() );
        d.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Hidden );
        m_visitedDirCnt = 1; // take the selected target-root dir into account
        visitDir(d, 0ULL); // Core function 

        qDebug() << "Elapsed tick = " << m_timerTotal.elapsed() / 1000.0 << " second(s).";

        qDebug() << "DirCnt = "  << m_visitedDirCnt;
        qDebug() << "FileCnt = " << m_visitedFileCnt;
        qDebug() << "MaxLayer = " << m_maxLayer;

        ui->stopBtn->hide();
        ui->stopBtn->setEnabled(false);

        ui->scanBtn->setEnabled( true );
        ui->clearBtn->setEnabled( true );


        fill_ScanResultIntoTreeView( );

        ui->visitResultTree->setEnabled( true );
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
        m_depthDirs.insert(0, QVector<QDir>({ QDir(d) }) );

        m_timerDirs.start();

        //
        // Run the background thread
        //
        m_multiThreadState = 1;
        // alloc a new thread object if necessary
        if ( m_pDirVisitThread == nullptr ) {
            m_pDirVisitThread = new myVisitThread( this );

            connect(m_pDirVisitThread, &myVisitThread::visitOneDir, this, &MainWindow::onVisitOneDir );
            connect(m_pDirVisitThread, &myVisitThread::selectAllDirs, this, &MainWindow::onVisitAllDirsFinished );
            connect(m_pDirVisitThread, &myVisitThread::finished, m_pDirVisitThread, &QObject::deleteLater );
        }

        m_pDirVisitThread->setType( myVisitThread::E_DIRS );
        // set dir filter ( if a certain dir matched the filter condition ( match with dir.startsWith(pattern) -> true ), !![Do NOT]!! take it into account ( !![DO NOT]!! Add it into the result dir list )  )
        m_pDirVisitThread->setDirIgnoreOption(m_bIgnoredFolderCaseSensitive,  m_ignoredFolderPattern, m_bOnlyVisitMatchedFolder );
        m_pDirVisitThread->setStartDir( d );
        // set travelsal symbol-link flag
        m_pDirVisitThread->setSkipSymbol_linkDirFlag(  m_bSkipSymbol_link_dirFlag );
        m_pDirVisitThread->setSkipSymbol_linkFileFlag( m_bSkipSymbol_link_fileFlag );


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

            ui->visitResultTree->setEnabled( true );
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

            ui->visitResultTree->setEnabled( true );
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
    static const QChar SC_BACK_SLASH('\\');
    static const QChar SC_SLASH('/');

    QString processed_path = ui->processingFolderPath->text();

    if ( processed_path.contains(SC_SLASH) ) {
        processed_path.replace( SC_SLASH, SC_BACK_SLASH );
    } else {
        processed_path.replace( SC_BACK_SLASH, SC_SLASH );
    }
    ui->processingFolderPath->setText( processed_path );
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
        if ( eachfInfo.isDir()  ) {
            if ( m_bSkipSymbol_link_dirFlag  && eachfInfo.isSymLink() ) {
                continue;
            }

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
                continue;
            }
        } else {
            if ( m_bPickFiles ) {
                // if ( m_bSkipSymbol_link_fileFlag && eachfInfo.isSymLink() ) {
                //     continue;
                // }

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
                            m_extensionMap[G_SC_NO_SUFFIX][""].push_back( eachfInfo );
                        } else {
                            m_extensionMap[G_SC_SUFFIX_IS_EMPTY][""].push_back( eachfInfo );
                        }
                    } else {
                        if ( completeSuffixExt != suffixExt ) {
                            m_extensionMap[G_SC_STR_DOT + suffixExt][G_SC_STR_DOT + completeSuffixExt].push_back( eachfInfo );
                        } else {
                            m_extensionMap[G_SC_STR_DOT  + suffixExt][""].push_back( eachfInfo ) ;
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
        m_depthDirs.insert( layer, QVector<QDir>({ QDir(path) }) );
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

    // such as  "Makefile"
    if ( completeSuffixExt.isEmpty() && suffixExt.isEmpty() ) {
        // such as  "/usr/hello." ( [Pay Attention] Please :  there has a '.' after the word "hello"  ), 
        // Both of completeSuffix() and suffix()  return  "" ( an empty string )
        if ( !fullpath.contains( G_SC_CHAR_DOT  ) ) {
            auto it = m_extensionMap.find(G_SC_NO_SUFFIX);
            m_extTypeGroupCnt     += (it == m_extensionMap.end() ? 1 : 0);

            m_extensionMap[G_SC_NO_SUFFIX][""].push_back( finfo );
        } else {
            auto it = m_extensionMap.find(G_SC_SUFFIX_IS_EMPTY);
            m_extTypeGroupCnt     += (it == m_extensionMap.end() ? 1 : 0);

            m_extensionMap[G_SC_SUFFIX_IS_EMPTY][""].push_back( finfo );
        }
    } else {
        if ( completeSuffixExt != suffixExt ) {
            QString shortSuffix = G_SC_STR_DOT + suffixExt;
            QString longSuffix  = G_SC_STR_DOT + completeSuffixExt;
            auto itShort = m_extensionMap.find(shortSuffix);
            if ( itShort == m_extensionMap.end() ) {
                m_extTypeGroupPartCnt += 1;
            } else {
                m_extTypeGroupPartCnt += ( itShort.value().find(longSuffix) != itShort.value().end() ? 0 : 1);
            }
            
            m_extensionMap[shortSuffix][longSuffix].push_back( finfo );
        } else {
            QString shortSuffix = G_SC_STR_DOT + suffixExt;
            auto itShort = m_extensionMap.find(shortSuffix);
            m_extTypeGroupCnt     += (itShort == m_extensionMap.end() ? 1 : 0);

            m_extensionMap[shortSuffix][""].push_back( finfo ) ;
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
    ui->visitResultTree->setEnabled( true );
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

    fill_fileGroupTreeNode();
    fill_dirGroupTreeNode();

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
    //                                  10 MB   =  1 KB     1 MB  * 10 =>  10 MB
    static const qint64 SC_FILE_READ_LIMIT_10MB = 1024ll * 1024ll * 10;

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
            QString realPath;
            if ( val == 1 ) {
                // File
                auto dirOrFile_Path = item->text(1);
                realPath = dirOrFile_Path;
                ui->processingFolderPath->setText( dirOrFile_Path );
                QFileInfo info( dirOrFile_Path );
                if ( info.isFile() ) {
                    QFile file( dirOrFile_Path );
                    if ( file.open( QIODevice::ReadOnly | QIODevice::ExistingOnly) ) {

                        qint64 ffSz = file.size();
                        QByteArray byteArray;
                        if ( ffSz <= SC_FILE_READ_LIMIT_10MB ) {
                            byteArray = file.readAll();
                        } else {
                            // read partially
                            byteArray = file.read( SC_FILE_READ_LIMIT_10MB );
                        }

                        const QRegExp re(".+\\.(bmp|gif|jpg|png|pbm|pgm|ppm|xbm|xpm|svg)$", Qt::CaseInsensitive);
                        // bool 
                        // contains(const QRegExp &rx) const
                        if ( dirOrFile_Path.contains(re) ) {
                            file.close();

                            if ( 0 ) {
                                qDebug() << "png hit " << dirOrFile_Path;
                                ui->fileContentView->setPlainText( QString("") );

                                // QUrl imageUrl( QString("file://%1").arg(dirOrFile_Path) );
                                // if ( imageUrl.isValid() ) {
                                //     qDebug() << "url is valid";
                                // } else {
                                //     qDebug() << "url is [NOT] valid";
                                // }

                                QString urlpath = QString("file://%1").arg(dirOrFile_Path);
                                // QString urlpath = QString("%1").arg(dirOrFile_Path);
                                QUrl imageUrl(urlpath);

                                // QPixmap icon(dirOrFile_Path); 
                                // QImage image = icon.toImage();
                                QImage image = QImageReader ( dirOrFile_Path ).read();
                                QTextDocument* textDocument = ui->fileContentView->document();
                                textDocument->addResource( QTextDocument::ImageResource, imageUrl, QVariant ( image ) );

                                QTextCursor cursor = ui->fileContentView->textCursor();
                                cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor );

                                // QTextImageFormat imageFormat;
                                // imageFormat.setWidth( image.width() );
                                // imageFormat.setHeight( image.height() );
                                // imageFormat.setName( imageUrl.toString() );

                                cursor.insertImage( image ); 
                                // cursor.insertImage( imageUrl.toString() );

                                // ui->fileContentView->setDocument( textDocument );
                                // ui->fileContentView->update();

                                ui->fileContentView->setTextCursor( cursor );
                                // ui->fileContentView->setDocument( textDocument );
                                // ui->fileContentView->update();

                                // qDebug() << "image = " << image;

                                // QVariant imageVar = ui->fileContentView->loadResource( QTextDocument::ImageResource, QUrl(imageUrl) );
                                // QImage loadedImage = QImage::fromData( byteArray );
                                // qDebug() << loadedImage;




                                // QTextDocument * textDocument = ui->fileContentView->document();
                                // textDocument->addResource( QTextDocument::ImageResource, imageUrl, QVariant ( image ) );

                                // QTextCursor cursor = ui->fileContentView->textCursor();
                                // QTextImageFormat imageFormat;
                                // imageFormat.setWidth( image.width() );
                                // imageFormat.setHeight( image.height() );
                                // imageFormat.setName( imageUrl.toString() );
                                // cursor.insertImage(imageFormat);
                                // ui->fileContentView->setTextCursor( cursor );

                                // ui->fileContentView->setDocument( textDocument );
                                ui->fileContentView->update();
                            } 




                            /*
                                QString file = QFileDialog::getOpenFileName(this, tr("Select an image"),
                                                              ".", tr("Bitmap Files (*.bmp)\n"
                                                                "JPEG (*.jpg *jpeg)\n"
                                                                "GIF (*.gif)\n"
                                                                "PNG (*.png)\n"));
                                QUrl Uri ( QString ( "file://%1" ).arg ( file ) );
                                QImage image = QImageReader ( file ).read();

                                QTextDocument * textDocument = m_textEdit->document();
                                textDocument->addResource( QTextDocument::ImageResource, Uri, QVariant ( image ) );
                                QTextCursor cursor = m_textEdit->textCursor();
                                QTextImageFormat imageFormat;
                                imageFormat.setWidth( image.width() );
                                imageFormat.setHeight( image.height() );
                                imageFormat.setName( Uri.toString() );
                                cursor.insertImage(imageFormat);
                            */

                            // QVariant var =  ui->fileContentView->loadResource( QTextDocument::ImageResource, QUrl( QString("file:///F:/2023-04-10扫墓/IMG_7729.jpg") ) );
                            // qDebug() << "var = " << var;
                        } else {
                            ui->fileContentView->setPlainText( QString(byteArray) );
                            // QVariant var = ui->targetScanTextBox->loadResource( QTextDocument::ImageResource, QUrl() );
                        }

                    } else {
                        ui->statusbar->clearMessage();
                        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
                        ui->statusbar->showMessage("[ERROR] : 打开文件失败", 5000);
                    }
                    file.close();
                }
            } else {
                realPath = item->text(0);
            } 

            ui->processingFolderPath->setText( realPath );
        }

    }
}



void MainWindow::fill_fileGroupTreeNode()
{
    // local storage to prevent it has been changed during the function fill_fileGroupTreeNode() executed.
    auto localbSkipSymbollinkFileFlag = m_bSkipSymbol_link_fileFlag;

    // Fill <File> Part
    QTreeWidgetItem *pFileRoot = new QTreeWidgetItem( ui->visitResultTree );
    pFileRoot->setText(0, "Files");
    pFileRoot->setText(1, QString("%1 file(s) of %2 Entire Ext Sorts").arg( m_visitedFileCnt ).arg( m_extensionMap.size() ) );
    if ( sc_b_USE_ICON ) { pFileRoot->setIcon(0, *m_treeFileIcon ); }

    if ( m_bPickFiles ) {
        auto extTypeidx = 0;
        for ( auto it = m_extensionMap.begin(); it!=m_extensionMap.end(); ++it ) {
            const auto strExtKey = it.key();
            const auto& maybe_MultiExtTree = it.value();
            // check if the given "strExtKey" has multipart ?
            if ( strExtKey != G_SC_NO_SUFFIX && strExtKey != G_SC_SUFFIX_IS_EMPTY ) { // display such item at last
                bool bhasMultiExtFlag = false;
                for ( auto subIt = maybe_MultiExtTree.begin(); subIt != maybe_MultiExtTree.end(); ++subIt ) {
                    const auto& realext = subIt.key();
                    if ( !realext.isEmpty()  ) {
                        bhasMultiExtFlag = true;
                        break;
                    }
                }


                int nKinds = 0;
                // a certain <Extension Root> Node
                QTreeWidgetItem *pFileSubExtTreeRoot = new QTreeWidgetItem( pFileRoot );
                pFileSubExtTreeRoot->setText(0, strExtKey );
                if ( sc_b_USE_ICON ) { pFileSubExtTreeRoot->setIcon(0, *m_treeExtIcon ); }


                QMap<QString, int> multiGroups;
                if ( bhasMultiExtFlag ) {
                    // Fixed <Mt-Root> root-Node
                    QTreeWidgetItem *oneMultiExtTreeRootWrap = new QTreeWidgetItem( pFileSubExtTreeRoot );
                    oneMultiExtTreeRootWrap->setText(0, G_SC_MULTI_ROOT);
                    // TODO ( Done ? ) , collected by directories    -->  oneMultiExtTreeRootWrap
                    if ( sc_b_USE_ICON ) { oneMultiExtTreeRootWrap->setIcon(0, *m_treeMtExtIcon ); }

                    int subMultiCnt = 0;
                    for ( auto subIt = maybe_MultiExtTree.begin(); subIt != maybe_MultiExtTree.end(); ++subIt ) {
                        const auto& realext = subIt.key();
                        const auto& fileInfoList = subIt.value();
                        if ( !realext.isEmpty()  ) {
                            ++nKinds;

                            // Real Node  such as   ".back.txt" 
                            QTreeWidgetItem *subMTextRoot = new QTreeWidgetItem( oneMultiExtTreeRootWrap );
                            subMTextRoot->setText(0, QString("#%1 %2").arg(subMultiCnt+1).arg(realext) );
                            QMap<QString, int> subGroups;
                            // fill real file list
                            for ( auto fIt = fileInfoList.begin(); fIt!=fileInfoList.end(); ++fIt ) {
                                // file Node
                                QTreeWidgetItem *pFile = new QTreeWidgetItem( subMTextRoot );
                                auto absInnerPath = fIt->absolutePath();
                                pFile->setText(0, QString("%1").arg( fIt->fileName() ) ); // fileName only
                                pFile->setText(1, QString("%1  %2").arg( absInnerPath  ).arg( fIt->fileName() ) ); // file path
                                if ( !fIt->isSymLink() ) {
                                    pFile->setText(2, QString("%1").arg( MainWindow::getHumanReadableSize( fIt->size() ) ) ); // file size
                                } else {
                                    if ( localbSkipSymbollinkFileFlag ) {
                                        pFile->setText(2, QString("file Symbol-Link : 0 bytes. ") ); 
                                    } else {
                                        pFile->setText(2, QString("file Symbol-Link : 0 bytes -> real: %1").arg( MainWindow::getHumanReadableSize( fIt->size() ) ) ); // file size
                                    }                                        
                                }

                                auto subInnerIt = subGroups.find( absInnerPath  );
                                if ( subInnerIt == subGroups.end() ) {
                                    subGroups[ absInnerPath ] = 1;
                                } else {
                                    ++( subInnerIt.value() );
                                }

                                if ( sc_b_USE_ICON ) { pFile->setIcon(0, *m_treeFileIcon  ); }
                                pFile->setFlags(pFile->flags() | Qt::ItemIsEditable );
                                // QVariant(1) -> file   |  QVariant(2) -> dir
                                pFile->setData(0,  Qt::UserRole, QVariant(1) );
                            }

                            QString groupSortMsg;
                            for ( auto siIt = subGroups.begin(); siIt != subGroups.end(); ++siIt ) {
                                groupSortMsg += QString("%1 : %2 file(s); ").arg( siIt.key() ).arg( siIt.value() );

                                auto ffIt = multiGroups.find( siIt.key() );
                                if ( ffIt != multiGroups.end() ) {
                                    ffIt.value() += siIt.value();
                                } else {
                                    multiGroups[ siIt.key() ] = siIt.value();
                                }
                            }

                            subMTextRoot->setText(1, QString("%1 groups : %2").arg( subGroups.size() ).arg( groupSortMsg ) );
                            subMTextRoot->setFlags( subMTextRoot->flags() | Qt::ItemIsEditable );
                            ++subMultiCnt;
                        }
                    }

                    QString multiSumMsg;
                    for( auto it1 = multiGroups.begin(); it1 != multiGroups.end(); ++it1 ) {
                        multiSumMsg += QString("%1 : %2 file(s); ").arg(  it1.key() ).arg( it1.value() );
                    }
                    oneMultiExtTreeRootWrap->setText(1, multiSumMsg);
                    oneMultiExtTreeRootWrap->setFlags( oneMultiExtTreeRootWrap->flags() | Qt::ItemIsEditable );
                } 

                //
                // fill None-Multi part ( normal part )
                //
                auto normalIt = maybe_MultiExtTree.find("");
                if ( normalIt != maybe_MultiExtTree.end() ) {
                    ++nKinds;
                    auto fList = normalIt.value();
                    for ( auto fIt = fList.begin(); fIt != fList.end() ; ++fIt ) {
                        QTreeWidgetItem *pFile = new QTreeWidgetItem( pFileSubExtTreeRoot );

                        pFile->setText(0, QString("%1").arg( fIt->fileName() ) );
                        pFile->setText(1, QString("%1  %2").arg( fIt->absolutePath() ).arg( fIt->fileName() ) );
                        if ( !fIt->isSymLink() ) {
                            pFile->setText(2, QString("%1").arg( MainWindow::getHumanReadableSize( fIt->size() ) ) ); // file size
                        } else {
                            if ( localbSkipSymbollinkFileFlag ) {
                                pFile->setText(2, QString("file Symbol-Link : 0 byte. ") );
                            } else {
                                pFile->setText(2, QString("file Symbol-Link : 0 byte -> %1 .").arg( MainWindow::getHumanReadableSize( fIt->size() ) ) ); // file size
                            }                                
                        }

                        if ( sc_b_USE_ICON ) { pFile->setIcon(0, *m_treeFileIcon  ); }
                        pFile->setFlags(pFile->flags() | Qt::ItemIsEditable );
                        // QVariant(1) -> file   |  QVariant(2) -> dir
                        pFile->setData(0,  Qt::UserRole, QVariant(1) );
                    }

                    multiGroups[ strExtKey ] = fList.size();
                }

                pFileSubExtTreeRoot->setText(1, QString("%1 sub kinds of \"%2\" ").arg( nKinds ).arg( strExtKey ) );

                QString multiFullMsg;
                for( auto it1 = multiGroups.begin(); it1 != multiGroups.end(); ++it1 ) {
                    multiFullMsg += QString("%1 : %2 file(s); ").arg(  it1.key() ).arg( it1.value() );
                }
                pFileSubExtTreeRoot->setText(2, QString("%1").arg( multiFullMsg ) );
                pFileSubExtTreeRoot->setFlags( pFileSubExtTreeRoot->flags() | Qt::ItemIsEditable );

                ++extTypeidx;
            } 
        }

        QMap<QString,int> suffixEmptyGroups;
        auto suffixIsEmptyItem_It = m_extensionMap.find(G_SC_SUFFIX_IS_EMPTY);
        // G_SC_SUFFIX_IS_EMPTY    such as a file named "hello."
        if ( suffixIsEmptyItem_It != m_extensionMap.end() ) {
            // suffixEmptyGroups
            QTreeWidgetItem *specialRoot1 = new QTreeWidgetItem( pFileRoot );
            specialRoot1->setText(0, G_SC_SUFFIX_IS_EMPTY);
            const auto& mp = suffixIsEmptyItem_It.value();
            const auto& fList = mp.find("").value();
            specialRoot1->setText(1, QString("%1 files").arg( fList.size() ) );

            for( auto fIt = fList.begin(); fIt != fList.end(); ++fIt ) {
                QTreeWidgetItem *pFile = new QTreeWidgetItem( specialRoot1 );

                pFile->setText(0, QString("%1").arg( fIt->fileName() ) );
                pFile->setText(1, QString("%1  %2").arg( fIt->absolutePath() ).arg( fIt->fileName() ) );
                auto foundIt1 = suffixEmptyGroups.find( fIt->absolutePath() );
                if ( foundIt1 == suffixEmptyGroups.end() ) {
                    suffixEmptyGroups[ fIt->absolutePath() ] = 1;
                } else {
                    ++( foundIt1.value() );
                }

                if ( !fIt->isSymLink() ) {
                    pFile->setText(2, QString("%1").arg( MainWindow::getHumanReadableSize( fIt->size() ) ) ); // file size
                } else {
                    if ( localbSkipSymbollinkFileFlag ) {
                        pFile->setText(2, QString("file Symbol-Link : 0 byte .") );
                    } else {
                        pFile->setText(2, QString("file Symbol-Link : 0 byte -> %1").arg( MainWindow::getHumanReadableSize( fIt->size() ) ) ); // file size
                    }
                }

                if ( sc_b_USE_ICON ) { pFile->setIcon(0, *m_treeFileIcon  ); }
                pFile->setFlags(pFile->flags() | Qt::ItemIsEditable );
                // QVariant(1) -> file   |  QVariant(2) -> dir
                pFile->setData(0,  Qt::UserRole, QVariant(1) );
            }

            QString summaryGroupMsg;
            for( auto it = suffixEmptyGroups.begin(); it != suffixEmptyGroups.end(); ++it ) {
                summaryGroupMsg += QString("%1 : %2 file(s); ").arg( it.key() ).arg( it.value() );
            }
            specialRoot1->setText(2, QString("%1").arg( summaryGroupMsg ) );
            specialRoot1->setFlags( specialRoot1->flags() | Qt::ItemIsEditable );
        }


        auto noSuffixItem_It      = m_extensionMap.find(G_SC_NO_SUFFIX);
        QMap<QString,int> noSuffixGroups;
        // G_SC_NO_SUFFIX    such as a file named "Makefile"
        if ( noSuffixItem_It != m_extensionMap.end() ) {
            QTreeWidgetItem *specialRoot2 = new QTreeWidgetItem( pFileRoot );
            specialRoot2->setText(0, G_SC_NO_SUFFIX);
            const auto& mp = noSuffixItem_It.value();
            const auto& fList = mp.find("").value();
            specialRoot2->setText(1, QString("%1 files").arg( fList.size() ) );

            for( auto fIt = fList.begin(); fIt != fList.end(); ++fIt ) {
                QTreeWidgetItem *pFile = new QTreeWidgetItem( specialRoot2 );

                pFile->setText(0, QString("%1").arg( fIt->fileName() ) );
                pFile->setText(1, QString("%1  %2").arg( fIt->absolutePath() ).arg( fIt->fileName() ) );

                auto foundIt2 = noSuffixGroups.find( fIt->absolutePath() );
                if ( foundIt2 == noSuffixGroups.end() ) {
                    noSuffixGroups[ fIt->absolutePath() ] = 1;
                } else {
                    ++( foundIt2.value() );
                }

                if ( !fIt->isSymLink() ) {
                    pFile->setText(2, QString("%1").arg( MainWindow::getHumanReadableSize( fIt->size() ) ) ); // file size
                } else {
                    if ( localbSkipSymbollinkFileFlag ) {
                        pFile->setText(2, QString("file Symbol-Link : 0 byte .") ); 
                    } else {
                        pFile->setText(2, QString("file Symbol-Link : 0 byte -> %1 .").arg( MainWindow::getHumanReadableSize( fIt->size() ) ) ); // file size
                    }                        
                }

                if ( sc_b_USE_ICON ) { pFile->setIcon(0, *m_treeFileIcon  ); }
                pFile->setFlags(pFile->flags() | Qt::ItemIsEditable );
                // QVariant(1) -> file   |  QVariant(2) -> dir
                pFile->setData(0,  Qt::UserRole, QVariant(1) );
            }

            QString summaryGroupMsg;
            for( auto it = noSuffixGroups.begin(); it != noSuffixGroups.end(); ++it ) {
                summaryGroupMsg += QString("%1 : %2 file(s); ").arg( it.key() ).arg( it.value() );
            }
            specialRoot2->setText(2, QString("%1").arg( summaryGroupMsg ) );
            specialRoot2->setFlags( specialRoot2->flags() | Qt::ItemIsEditable );
        }
    }
}



void MainWindow::fill_dirGroupTreeNode()
{
    // Fill <Dir> Part
    QTreeWidgetItem *pDirRoot = new QTreeWidgetItem( ui->visitResultTree );
    pDirRoot->setText(0, "Dirs");
    pDirRoot->setText(1, QString("%1 count of %2 types").arg(m_visitedDirCnt).arg(m_bPickDirs ?  m_pAllDirs->size() : 0 ) );
    if ( sc_b_USE_ICON ) { pDirRoot->setIcon(0, *m_treeDirIcon  ); }

    if ( m_bPickDirs ) {
        if ( m_pAllDirs != nullptr && !m_pAllDirs->isEmpty() ) {
            QMap<QString, QVector<QDir> > groups;

            for ( auto it = m_pAllDirs->begin(); it!=m_pAllDirs->end(); ++it ) {
                groups[ it->dirName()  ].push_back( *it );
            }

            auto idx = 0;
            for( auto it = groups.begin(); it!=groups.end(); ++it, ++idx ) {
                QTreeWidgetItem *pDirType = new QTreeWidgetItem( pDirRoot );
                pDirType->setText(0, QString("#%1 %2").arg(idx+1).arg( it.key() ) );
                pDirType->setText(1, QString("%1 with same name").arg(it.value().size()) );
                if ( sc_b_USE_ICON ) { pDirType->setIcon(0, *m_treeDirIcon  ); }

                for( auto it2 = it.value().begin(); it2!=it.value().end(); ++it2 ) {
                    QTreeWidgetItem *pDirObj = new QTreeWidgetItem( pDirType );

                    m_generatedTreeNodeList.push_back( pDirObj );

                    pDirObj->setText(0, QString("%1").arg( it2->absolutePath() ) );
                    pDirObj->setFlags(pDirObj->flags() | Qt::ItemIsEditable );

                    // QVariant(1) -> file   |  QVariant(2) -> dir
                    pDirObj->setData(0, Qt::UserRole, QVariant(2) );

                    if ( sc_b_USE_ICON ) { pDirObj->setIcon(0, *m_treeDirIcon  ); }
                }
            }
        }
    }
}




// static util helper function(s)
QString MainWindow::getHumanReadableSize(qint64 fileSz)
{
    static const qint64 SC_RATE   = 1024ll;
    static const qint64 SC_ONE_KB = 1ll * 1024ll;
    static const qint64 SC_ONE_MB = SC_ONE_KB * SC_RATE;
    static const qint64 SC_ONE_GB = SC_ONE_MB * SC_RATE;
    static const qint64 SC_ONE_TB = SC_ONE_GB * SC_RATE;
    static const qint64 SC_ONE_PB = SC_ONE_TB * SC_RATE;

    QString strSZret;
    if ( fileSz < SC_ONE_KB ) {
        strSZret = QString("%1 bytes").arg( fileSz );
    } else if ( fileSz < SC_ONE_MB ) {
        strSZret = QString("%1 KB -> ( %2 bytes )").arg( fileSz * 1.0f / SC_ONE_KB ).arg( fileSz );
    } else if ( fileSz < SC_ONE_GB ) {
        strSZret = QString("%1 MB -> ( %2 bytes )").arg( fileSz * 1.0f / SC_ONE_MB ).arg( fileSz );
    } else if ( fileSz < SC_ONE_TB ) {
        strSZret = QString("%1 GB -> ( %2 bytes )").arg( fileSz * 1.0f / SC_ONE_GB ).arg( fileSz );
    } else if ( fileSz < SC_ONE_PB ) {
        strSZret = QString("%1 TB -> ( %2 bytes )").arg( fileSz * 1.0f / SC_ONE_TB ).arg( fileSz );
    } else {
        strSZret = QString("%1 PB -> ( %2 bytes )").arg( fileSz * 1.0f / SC_ONE_PB ).arg( fileSz );
    }

    return strSZret;
}



void MainWindow::on_actionSkipDir_triggered()
{
    m_bSkipSymbol_link_dirFlag = !m_bSkipSymbol_link_dirFlag;
    ui->actionSkipDir->setChecked( m_bSkipSymbol_link_dirFlag );

    QFileInfo fl("G:/aaTEST/wrap/EndeavourOS-3.jpg.lnk");
    if ( fl.exists() ) {
        qDebug() << "it has already been existed.";
    } else {
        qDebug() << "it is Empty.";
    }

    qDebug() << "size = " << fl.size();
}


void MainWindow::on_actionSkipFile_triggered()
{
    m_bSkipSymbol_link_fileFlag = !m_bSkipSymbol_link_fileFlag;
    ui->actionSkipFile->setChecked( m_bSkipSymbol_link_fileFlag );
}



