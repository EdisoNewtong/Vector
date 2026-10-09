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
    , m_targetFiles("")
    , m_cmpSubType( 0 )
    , m_dirs()
    , m_files()
    , m_createdDirs()
    , m_randomDirVisitProcessNumber(0)
    , m_reached_max_layer(0)
    , m_fileCnt(0)
    , m_timer_i()
    , m_timer_total()
    , m_pEncDecObj( nullptr )
    , m_op( E_OP_TYPE::E_NOP )
    , m_bIsTargetFileReady( false )
    , m_bPrintFileListOnly( false )
    , m_bBuildSameDirStructOnly( false )
    , m_bTakeEncActionFlag( true )
    , m_bDecOnSameFileFlag( false )
    , m_bDecAndRenameFileFlag( false )
{

}

// virtual
WorkThread::~WorkThread() // Q_DECL_OVERRIDE
{

}

void WorkThread::fillFileList(const QString& fileListByLine, int subType /* = 0 */ )
{
    m_targetFiles = fileListByLine;
    m_cmpSubType = subType;
    m_bIsTargetFileReady = true;
}
    
    
void WorkThread::setPath(const QString& original,const QString& dest, FNEDCore* pObj)
{
    m_oriDir  = original;
    m_destDir = dest;

    m_pEncDecObj = pObj;

    m_bIsTargetFileReady = false;
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


QString WorkThread::getPrint_EncFileList()
{
    QString fileTreeStr;

    for ( auto it = m_files.begin(); it != m_files.end(); ++it ) {
        const auto& layerFiles = it.value();
        for ( auto fIt = layerFiles.begin(); fIt != layerFiles.end(); ++fIt ) {
            fileTreeStr += *fIt;
            fileTreeStr += "\n";
        }
    }

    return fileTreeStr;

}


    
void WorkThread::singleThreadRun()
{
    if ( m_op == E_OP_TYPE::E_ENC_OP ) {
        do_EncOp();
        emit encdecFilesFinished("Encrypt files Finished");
    } else if ( m_op == E_OP_TYPE::E_DEC_OP ) {
        do_DecOp();
        emit encdecFilesFinished("Decrypt files Finished");
    } else if ( m_op == E_OP_TYPE::E_CMP_OP ) {
        do_CmpOp();
        emit encdecFilesFinished("Compare files Finished");
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

            // if ( m_bPrintFileListOnly ) {
            //     emit aNewFileComes ( entry.absoluteFilePath() );
            // }

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
            errorLog = QString("%1. create %2 dir Failed.").arg( idx+1 ).arg( relPath );
            emit updateFileListProcessState( errorLog );
        } 
    }

}

void WorkThread::process_EncTravesalFileList()
{
    int fileIdx = 0;
    int failedCnt = 0;

    m_timer_total.start();
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
                logInfo = QString("[SUCC] Encrypt the given file %1 Successfully! elapsed time : %2").arg( newGenFileName  ).arg( sec2min2hour(m_timer_i.elapsed() / 1000.0)  );
            } else {
                ++failedCnt;
                logInfo = QString("%1 | Encrypt elapsed time : %2.").arg(error).arg(  sec2min2hour( m_timer_i.elapsed() / 1000.0 ) );
            }

            // one file processed
            emit updateFileListProcessState( logInfo );
            emit updateFileListPrograssBar( (fileIdx+1) * 1.0f / m_fileCnt * 100.0f );
        }
    }

    if ( failedCnt == 0 ) {
        emit updateFileListProcessState( QString("[Finished] All Encrypt is Successfully %1/%2. Totally elapsed %3. ").arg( m_fileCnt ).arg( m_fileCnt ).arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
    m_timer_total.start();
    } else {
        emit updateFileListProcessState( QString("[Finished] Someone Encrypt went [FAILED] %1/%2 Failed. Totally elapsed %3.").arg( failedCnt ).arg( m_fileCnt ).arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
    }
}




void WorkThread::process_DecFileList()
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
                logInfo = QString("[SUCC] Decrypt the given file %1 Successfully! elapsed time : %2").arg( iFilePath ).arg( sec2min2hour(m_timer_i.elapsed() / 1000.0)  );
            } else {
                ++failedCnt;
                logInfo = QString("%1 | Decrypt elapsed time : %2.").arg(error).arg(  sec2min2hour( m_timer_i.elapsed() / 1000.0 ) );
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




void WorkThread::setOp(E_OP_TYPE op)
{
    m_op = op;
}





void WorkThread::setPrintFilesOnlyFlag(bool b)
{
    m_bPrintFileListOnly = b;
}


void WorkThread::setBuildDirOnly(bool b)
{
    m_bBuildSameDirStructOnly = b;
}

void WorkThread::setTake_EncryptActionFlag(bool b)
{
    m_bTakeEncActionFlag = b;
}
    



void WorkThread::setDecOnSameFileFlag(bool  b) 
{
    m_bDecOnSameFileFlag = b;
}

void WorkThread::setDecAndRenameFileFlag(bool  b) 
{
    m_bDecAndRenameFileFlag = b;
}






void WorkThread::do_EncOp()
{
    if ( m_bIsTargetFileReady ) {
        if ( m_bTakeEncActionFlag ) {
            int fileIdx = 0;
            int failedCnt = 0;

            m_timer_total.start();
            auto fileList = m_targetFiles.split( QChar('\n'), QString::SkipEmptyParts );
            for ( auto fIt = fileList.begin(); fIt != fileList.end(); ++fIt, ++fileIdx ) {
                auto fileAbsPath = fIt->trimmed();

                m_timer_i.start();

                QString error;
                QString logInfo;

                if ( m_pEncDecObj!=nullptr && m_pEncDecObj->encryptFile( fileAbsPath,  error ) ) {
                    logInfo = QString("[SUCC] Encrypt the given file %1 Successfully! elapsed time : %2").arg( fileAbsPath ).arg( sec2min2hour(m_timer_i.elapsed() / 1000.0)  );
                } else {
                    ++failedCnt;
                    logInfo = QString("%1 | Encrypt elapsed time : %2.").arg(error).arg(  sec2min2hour( m_timer_i.elapsed() / 1000.0 ) );
                }

                // one file processed
                emit updateFileListProcessState( logInfo );
                emit updateFileListPrograssBar( (fileIdx+1) * 1.0f / m_fileCnt * 100.0f );
            }

            if ( failedCnt == 0 ) {
                emit updateFileListProcessState( QString("[Finished] All Encrypt is Successfully %1/%2. Totally elapsed %3.").arg( fileIdx ).arg( fileIdx  ).arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
            } else {
                emit updateFileListProcessState( QString("[Finished] Someone Encrypt went [FAILED] %1/%2 Failed. Totally elapsed %3.").arg( failedCnt ).arg( fileIdx ).arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
            }
        } else {
            emit updateFileListProcessState( QString("[Finished] No need to take Encrypt Action. Totally elapsed %1.").arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
        }
    } else {
        // Step #0
        initVaribles();

        // Step #1 : visit original dir and prepare the file list
        visitDir();

        if ( m_bPrintFileListOnly ) {
            emit printDirTree( getPrint_EncFileList() );
        }

        if ( m_bBuildSameDirStructOnly ) {
            buildNewDirsOnDestintion();
        }

        if ( m_bTakeEncActionFlag ) {
            process_EncTravesalFileList();
        }
    }
}



void WorkThread::do_DecOp()
{
    if ( m_bIsTargetFileReady ) {
        int fileIdx = 0;
        int failedCnt = 0;

        auto fileList = m_targetFiles.split( QChar('\n'), QString::SkipEmptyParts );
        m_timer_total.start();
        for ( auto fIt = fileList.begin(); fIt != fileList.end(); ++fIt, ++fileIdx ) {
            m_timer_i.start();
            auto fileAbsPath = fIt->trimmed();


            QString error;
            QString logInfo;

            if ( m_pEncDecObj!=nullptr ) {
                // QString newFileName;
                // auto bret =  m_pEncDecObj->decryptFileRestore( fileAbsPath, error );
                //             ? m_pEncDecObj->decryptFileRestore( fileAbsPath, error )
                //             : m_pEncDecObj->decryptFile_dest( fileAbsPath, m_oriDir, m_destDir,  newFileName, error );

                auto bret = m_pEncDecObj->decryptFileRestore( fileAbsPath, error );
                if ( bret ){
                    logInfo = QString("[SUCC] Decrypt the given file %1  Successfully! elapsed time : %2").arg( fileAbsPath ).arg( sec2min2hour(m_timer_i.elapsed() / 1000.0)  );

                    if ( m_bDecAndRenameFileFlag ) {
                        auto currentTargetFileName = fileAbsPath;
                        auto bChnameFlag = FNEDCore::revert2OriginalName( currentTargetFileName );
                        qDebug() << QString("Rename %1 on \"%2\"").arg( bChnameFlag ? "[SUCC]" : "[FAILED]").arg( currentTargetFileName );
                    } 

                } else {
                    ++failedCnt;
                    logInfo = QString("%1 | Decrypt elapsed time : %2.").arg(error).arg(  sec2min2hour( m_timer_i.elapsed() / 1000.0 ) );
                }
            } 

            // one file processed
            emit updateFileListProcessState( logInfo );
            emit updateFileListPrograssBar( (fileIdx+1) * 1.0f / m_fileCnt * 100.0f );
        }

        if ( failedCnt == 0 ) {
            emit updateFileListProcessState( QString("[Finished] All Decrypt is Successfully %1/%2. Totally elapsed %3.").arg( fileIdx ).arg( fileIdx  ).arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
        } else {
            emit updateFileListProcessState( QString("[Finished] Someone Decrypt went [FAILED] %1/%2 Failed. Totally elapsed %3.").arg( failedCnt ).arg( fileIdx ).arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
        }
    } else {
        // Step #0
        initVaribles();

        // Step #1 : visit original dir and prepare the file list
        visitDir();

        if ( !m_bDecOnSameFileFlag  ) {
            buildNewDirsOnDestintion();
        } 

        process_DecTravesalFileList();
    }
}


void WorkThread::do_CmpOp()
{
    if ( m_cmpSubType == 0 ) {
        return;
    }

    auto fileList = m_targetFiles.split( QChar('\n'), QString::SkipEmptyParts );
    QVector< QPair<QString,QString> > filePairList;
    m_timer_total.start();

    if ( m_cmpSubType == 1 ) {
        // Compare 2 dirs
        // TODO :

        // Step #0
        initVaribles();

        // Step #1 : visit original dir and prepare the file list
        visitDir();


    } else if ( m_cmpSubType == 2 ) {
        // a lot of pairs to compare , each file with 2 files seperated by ';'
        for( auto it = fileList.cbegin(); it != fileList.cend() ; ++it ) {
            auto prs = it->split( QChar(';'), QString::SkipEmptyParts );
            if ( prs.size() == 2 ) {
                filePairList.push_back( qMakePair(prs[0], prs[1]) );
            }
        }
    } else if ( m_cmpSubType == 3 ) {
        // only 2 files paths are specified on 2 adjucent lines 
        auto lineSZ = fileList.size();
        if ( lineSZ == 2 ) {
            filePairList.push_back( qMakePair( fileList[0], fileList[1] )  );
        } else {
            emit updateFileListProcessState( QString("[Finished] Required 2 lines , but %1 lines is given. Totally used %2").arg( lineSZ ).arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
            return;
        }
    }

    if ( filePairList.empty() ) {
        emit updateFileListProcessState( QString("[Finished] 0 pairs. Totally elapsed %1.").arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
        return;
    }

    int pairSZ = filePairList.size();
    int totalCnt = 0;
    int differentCnt = 0;
    
    for( auto it = filePairList.cbegin(); it != filePairList.cend(); ++it, ++totalCnt ) {
        m_timer_i.start();

        QString errorMsg;
        auto b = FNEDCore::is2FileEqual( it->first, it->second, errorMsg);
        if ( b ) {
            emit updateFileListProcessState( QString("[SUCC] Compared Same files %1 = %2 files! elapsed time : %3.").arg( it->first ).arg( it->second ).arg( sec2min2hour( m_timer_i.elapsed() / 1000.0) ) );
        } else {
            ++differentCnt;
            emit updateFileListProcessState( QString("%1 | Compare 2 files elapsed time : %2").arg( errorMsg ).arg( sec2min2hour( m_timer_i.elapsed() / 1000.0) ) );
        }
        // update progress
        emit updateFileListPrograssBar( (totalCnt+1) * 1.0f / pairSZ * 100.0f );
    }

    if ( differentCnt == 0 ) {
        emit updateFileListProcessState( QString("[Finished] All compared %1/%2 files pair are equal. Totally elapsed time : %3.").arg( pairSZ ).arg( pairSZ ).arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
    } else {
        emit updateFileListProcessState( QString("[Finished] Compared %1/%2 pairs are same. Totally elapsed %3. ").arg( pairSZ-differentCnt ).arg( pairSZ ).arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
    }
}



void WorkThread::process_DecTravesalFileList()
{
    int fileIdx = 0;
    int failedCnt = 0;

    m_timer_total.start();
    for ( auto it = m_files.begin(); it != m_files.end(); ++it ) {
        const auto& layerFiles = it.value();
        for ( auto fIt = layerFiles.begin(); fIt != layerFiles.end(); ++fIt, ++fileIdx ) {
            m_timer_i.start();

            QString error;
            QString logInfo;

            auto iFilePath = *fIt;

            QString newGenFileName;
            if ( m_pEncDecObj!=nullptr ) {   
                auto bret =   m_bDecOnSameFileFlag 
                            ? m_pEncDecObj->decryptFileRestore( iFilePath, error )
                            : m_pEncDecObj->decryptFile_dest( iFilePath, m_oriDir, m_destDir,  newGenFileName, error );

                if ( bret ) {
                    logInfo = QString("[SUCC] Decrypt the given file %1 Successfully! elapsed time : %2").arg( m_bDecOnSameFileFlag ? iFilePath : newGenFileName ).arg( sec2min2hour(m_timer_i.elapsed() / 1000.0)  );
                    if ( m_bDecAndRenameFileFlag ) {
                        auto currentTargetFileName = m_bDecOnSameFileFlag ? iFilePath : newGenFileName;
                        auto bChnameFlag = FNEDCore::revert2OriginalName( currentTargetFileName );
                        qDebug() << QString("Rename %1 on \"%2\"").arg( bChnameFlag ? "[SUCC]" : "[FAILED]").arg( currentTargetFileName );
                    } 
                } else {
                    ++failedCnt;
                    logInfo = QString("%1 | Decrypt elapsed time : %2.").arg(error).arg(  sec2min2hour( m_timer_i.elapsed() / 1000.0 ) );
                }
            }

            // one file processed
            emit updateFileListProcessState( logInfo );
            emit updateFileListPrograssBar( (fileIdx+1) * 1.0f / m_fileCnt * 100.0f );
        }
    }

    if ( failedCnt == 0 ) {
        emit updateFileListProcessState( QString("[Finished] All Decrypt is Successfully %1/%2. Totally elapsed %3. ").arg( m_fileCnt ).arg( m_fileCnt ).arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
    } else {
        emit updateFileListProcessState( QString("[Finished] Someone Decrypt went [FAILED] %1/%2 Failed. Totally elapsed %3.").arg( failedCnt ).arg( m_fileCnt ).arg( sec2min2hour( m_timer_total.elapsed() / 1000.0) ) );
    }

}



