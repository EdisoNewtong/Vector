#include "workthread.h"

#include <QRandomGenerator>
#include <QDir>
#include <QDebug>

QString sec2min2hour(double sec)
{
    QString retStr;
    static const double hour = 3600.0;
    static const double min = 60.0;
    if ( sec >= hour ) {
        int hs = static_cast<int>( sec / hour);
        double rest = sec - hs*hour;
        int rmin = static_cast<int>( rest/min);
        double restSec = rest - rmin*min;
        retStr = QString("%1 hour(s) %2 min(s) %3 s. ").arg(hs).arg(rmin).arg(restSec);
    } else if ( sec >= min ) {
        int ms = static_cast<int>(sec / min);
        retStr = QString("%1 min(s) and %2 s ").arg(ms).arg( sec - ms*min );
    } else {
        retStr = QString("%1 s").arg( sec );
    }
    return retStr;
}

WorkThread::WorkThread(QObject* parent /* = nullptr */)
    : QThread(parent)
    , m_oriDir("")
    , m_destDir("")
    , m_dirs()
    , m_files()
    , m_createdDirs()
    , m_randomDirVisitProcessNumber(0)
    , m_reached_max_layer(0)
    , m_fileCnt(0)
    , m_timer_i()
    , m_pEncDecObj( nullptr )
    , m_op( E_OP_TYPE::E_NOP )
    , m_bPrintFileListOnly( false )
    , m_bBuildSameDirStructOnly( false )
{

}

// virtual
WorkThread::~WorkThread() // Q_DECL_OVERRIDE
{

}
    
    
void WorkThread::setPath(const QString& original,const QString& dest, FNEDCore* pObj)
{
    m_oriDir  = original;
    m_destDir = dest;

    m_pEncDecObj = pObj;
}

    
void WorkThread::initVaribles()
{
    m_dirs.clear();
    m_files.clear();
    m_createdDirs.clear();

    m_randomDirVisitProcessNumber = QRandomGenerator::global()->bounded(25, 36); // [25,35]
    m_reached_max_layer = 0;
    m_fileCnt = 0;
}

    
void WorkThread::singleThreadRun()
{
    // Step #0
    initVaribles();

    // Step #1 : visit original dir and prepare the file list
    visitDir();
    // intermeidiat step : make new directory struct inside destination dir  like original dir struct

    if ( !m_bPrintFileListOnly ) {
        // Step #1.5  Build Same Dir Struct 
        if ( m_op == E_OP_TYPE::E_ENC_OP ) {
            buildNewDirsOnDestintion();
        }

        if ( m_bBuildSameDirStructOnly ) {
            emit encdecFilesFinished("Build \"same dir struct\" Finished");
            return;
        } 

        // Step #2 : process each file in the file list in previous Step #1
        if ( m_op == E_OP_TYPE::E_ENC_OP ) {
            processEncFileList();
            emit encdecFilesFinished("Enc Finished");
        } else if ( m_op == E_OP_TYPE::E_DEC_OP ) {
            processDecFileList();
            emit encdecFilesFinished("Dec Finished");
        } else if ( m_op == E_OP_TYPE::E_CMP_OP ) {
            processCmpFileList();
            emit encdecFilesFinished("Cmp Finished");
        }
    }


}
 

// virtual 
void WorkThread::run() // Q_DECL_OVERRIDE;
{
    singleThreadRun();
}

    
void WorkThread::visitDir()
{
    travelsalDirWithLayer( m_oriDir, 0 );

    qDebug() << "m_reached_max_layer = " << m_reached_max_layer;
    qDebug() << "m_fileCnt = " << m_fileCnt;
    // emit travelsalEnd( ... )
}


    
void WorkThread::travelsalDirWithLayer( const QString& path, int layer )
{
    QDir dPath(path);

    auto dIt = m_dirs.find( layer );
    if ( dIt == m_dirs.end() ) {
        m_dirs.insert( layer, QSet<QString>{ path });
    } else {
        dIt.value().insert( path );
    }

    // update max layer
    if( layer > m_reached_max_layer ) {
        m_reached_max_layer = layer;
    }
    // emit a_new_dir_is_comming(dirPath, layer);

    QDir::Filters fltFlag =    QDir::AllEntries
                             | QDir::NoDotAndDotDot
                             | QDir::Hidden
                             | QDir::System
                             | QDir::CaseSensitive;

    // QDir::SortFlag sortFlag = QDir::DirsFirst;
    QDir::SortFlag sortFlag = QDir::NoSort;

    QFileInfoList children = dPath.entryInfoList(fltFlag, sortFlag);
    int children_cnt = children.size();

    int subDirCnt   = 0;
    int subFilesCnt = 0;
    for ( auto i = 0; i < children_cnt; ++i ) {
        auto entry = children.at(i);

        auto bIsDir = entry.isDir();
        auto bIsFile = entry.isFile();
        // auto bIsSymlink = entry.isSymLink();

        if ( bIsDir ) {
            //////////////////////////////////////////////////
            //
            // recursive call
            //
            //////////////////////////////////////////////////
            ++subDirCnt;
            travelsalDirWithLayer( entry.absoluteFilePath(), layer + 1);
        } else if ( bIsFile ) {
            ++subFilesCnt;
            ++m_fileCnt;
            // qDebug() << entry.absoluteFilePath();
            if ( m_bPrintFileListOnly ) {
                emit aNewFileComes ( entry.absoluteFilePath() );
            }

            // Added into file list
            auto it =  m_files.find( layer );
            if ( it == m_files.end() ) {
                // QSet<QString> files{ entry.absoluteFilePath() };
                m_files.insert( layer, QSet<QString>{ entry.absoluteFilePath() } ); 
            } else {
                it.value().insert( entry.absoluteFilePath()  );
            }
        } 
        /*
        else {
            // neither <Dir> Nor <File>
        }
        */
    }

    // Core Core Core :
    //   Insert dirs need to be created
    //
    if ( subDirCnt == 0 ) {
        m_createdDirs.insert( path );
    }
    // emit a_new_dir_is_fully_travesaled(dirPath, layer);

}



void WorkThread::buildNewDirsOnDestintion()
{
    QDir parentOfOriginal( m_oriDir );
    parentOfOriginal.cdUp(); // to its parent dir

    QDir destiDir( m_destDir );
    int idx = 0;
    // int createdCnt = m_createdDirs.size();
    for( auto it = m_createdDirs.begin(); it != m_createdDirs.end(); ++it , ++idx  ) {
        // calculate relative-path
        auto relPath = parentOfOriginal.relativeFilePath( *it );

        QString errorLog;
        if ( !destiDir.mkpath( relPath ) ) {
            errorLog = QString("%1. create %2 dir Failed.").arg( idx+1).arg( relPath );
            emit updateFileListProcessState( errorLog );
        } 
    }

}

void WorkThread::processEncFileList()
{
    int fileIdx = 0;
    int failedCnt = 0;

    for ( auto it = m_files.begin(); it != m_files.end(); ++it ) {
        const auto& layerFiles = it.value();
        for ( auto fIt = layerFiles.begin(); fIt != layerFiles.end(); ++fIt, ++fileIdx ) {
            m_timer_i.start();

            QString error;
            QString logInfo;

            auto iFilePath = *fIt;
            // qDebug() << (fileIdx+1) << ". " << iFilePath;

            QString newGenFileName;
            if ( m_pEncDecObj!=nullptr && m_pEncDecObj->encryptFile_dest( iFilePath, m_oriDir, m_destDir,  newGenFileName, error ) ) {
                logInfo = QString("[SUCC] Encode the given file %1 Successfully! elapsed time : %2").arg( newGenFileName  ).arg( sec2min2hour(m_timer_i.elapsed() / 1000.0)  );
            } else {
                ++failedCnt;
                logInfo = QString("%1 | Encode elapsed time : %2.").arg(error).arg(  sec2min2hour( m_timer_i.elapsed() / 1000.0 ) );
            }

            // one file processed
            emit updateFileListProcessState( logInfo );
            emit updateFileListPrograssBar( (fileIdx+1) * 1.0f / m_fileCnt * 100.0f );
        }
    }

    if ( failedCnt == 0 ) {
        emit updateFileListProcessState( QString("[Finished] All Enc is Successfully %1/%2").arg( m_fileCnt ).arg( m_fileCnt ) );
    } else {
        emit updateFileListProcessState( QString("[Finished] Someone Enc went [FAILED] %1/%2 Failed").arg( failedCnt ).arg( m_fileCnt ) );
    }
}




void WorkThread::processDecFileList()
{
    int fileIdx = 0;
    int failedCnt = 0;

    for ( auto it = m_files.begin(); it != m_files.end(); ++it ) {
        const auto& layerFiles = it.value();
        for ( auto fIt = layerFiles.begin(); fIt != layerFiles.end(); ++fIt, ++fileIdx ) {
            m_timer_i.start();

            QString error;
            QString logInfo;

            auto iFilePath = *fIt;
            // qDebug() << (fileIdx+1) << ". " << iFilePath;

            QString newGenFileName;
            if ( m_pEncDecObj!=nullptr && m_pEncDecObj->decryptFileRestore( iFilePath, error ) ) {
                logInfo = QString("[SUCC] Decode the given file %1 Successfully! elapsed time : %2").arg( iFilePath ).arg( sec2min2hour(m_timer_i.elapsed() / 1000.0)  );
            } else {
                ++failedCnt;
                logInfo = QString("%1 | Decode elapsed time : %2.").arg(error).arg(  sec2min2hour( m_timer_i.elapsed() / 1000.0 ) );
            }

            // one file processed
            emit updateFileListProcessState( logInfo );
            emit updateFileListPrograssBar( (fileIdx+1) * 1.0f / m_fileCnt * 100.0f );

        }
    }

    if ( failedCnt == 0 ) {
        emit updateFileListProcessState( QString("[Finished] All Dec is Successfully %1/%2").arg( m_fileCnt ).arg( m_fileCnt ) );
    } else {
        emit updateFileListProcessState( QString("[Finished] Someone Dec went [FAILED] %1/%2 Failed").arg( failedCnt ).arg( m_fileCnt ) );
    }
}


void WorkThread::processCmpFileList()
{
    int fileIdx = 0;
    int failedCnt = 0;

    for ( auto it = m_files.begin(); it != m_files.end(); ++it ) {
        const auto& layerFiles = it.value();
        for ( auto fIt = layerFiles.begin(); fIt != layerFiles.end(); ++fIt, ++fileIdx ) {
            m_timer_i.start();

            QString error;
            QString logInfo;

            auto iFilePath = *fIt;
            QDir oriDir( m_oriDir );
            QString relfilePath = oriDir.relativeFilePath(iFilePath);

            QDir destDir( m_destDir );
            QString iDestiFilePath =  destDir.absoluteFilePath ( relfilePath );
            // qDebug() << (fileIdx+1) << ". " << iDestiFilePath;

            if ( m_pEncDecObj!=nullptr && m_pEncDecObj->is2FileEqual(iFilePath, iDestiFilePath, error)  ) {
                logInfo = QString("[SUCC] Compare the given file %1 Successfully! elapsed time : %2").arg( iFilePath ).arg( sec2min2hour(m_timer_i.elapsed() / 1000.0)  );
            } else {
                ++failedCnt;
                logInfo = QString("%1 | Compare elapsed time : %2.").arg(error).arg(  sec2min2hour( m_timer_i.elapsed() / 1000.0 ) );
            }

            // one file processed
            emit updateFileListProcessState( logInfo );
            emit updateFileListPrograssBar( (fileIdx+1) * 1.0f / m_fileCnt * 100.0f );

        }
    }

    if ( failedCnt == 0 ) {
        emit updateFileListProcessState( QString("[Finished] All Cmp is Successfully %1/%2").arg( m_fileCnt ).arg( m_fileCnt ) );
    } else {
        emit updateFileListProcessState( QString("[Finished] Someone Cmp went [FAILED] %1/%2 Failed").arg( failedCnt ).arg( m_fileCnt ) );
    }
}



    
void WorkThread::setPrintFilesOnlyFlag(bool b)
{
    m_bPrintFileListOnly = b;
}


void WorkThread::setBuildDirOnly(bool b)
{
    m_bBuildSameDirStructOnly = b;
}
    
void WorkThread::setOp(E_OP_TYPE op)
{
    m_op = op;
}




