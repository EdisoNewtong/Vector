#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QRandomGenerator> 
#include <QDebug> 
#include <QDir> 

static const char SC_CH_R('\r');
static const char SC_CH_N('\n');

static const qint64 MB_10 = 1024ull * 1024ull * 10ull;    // 10 MB
static const qint64 MB_512 = 1024ull * 1024ull * 512ull;  // 512 MB


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_modeBtnAry{ nullptr, nullptr }
    , m_originalBtnAry{ nullptr, nullptr, nullptr, nullptr, nullptr }
    , m_convertBtnAry{ nullptr, nullptr, nullptr  }
    , m_EOL_NAME { 
        "Unix \\n", 
        "Win \\r\\n", 
        "Mac \\r",
        "HyBrid ??",
        "Not-Decided" 
    } 
    , m_eolType(E_NOT_DETECHED)
    , m_bReadState(0)
    , m_bLastEOLFlag( false )
    , m_loadedFilePath("")
{
    ui->setupUi(this);

    m_modeBtnAry[E_LAZY_MODE] = ui->lazyMode;
    m_modeBtnAry[E_FULL_MODE] = ui->fullMode;

    m_originalBtnAry[E_UNIX]   = ui->rdLinux;
    m_originalBtnAry[E_WIN]    = ui->rdWindows;
    m_originalBtnAry[E_MAC]    = ui->rdMacOS;
    m_originalBtnAry[E_HYBRID] = ui->rdHyBrid;
    m_originalBtnAry[E_NOT_DETECHED] = ui->rdReady;

    m_convertBtnAry[E_UNIX] = ui->cvtLinux; 
    m_convertBtnAry[E_WIN] = ui->cvtWindows;
    m_convertBtnAry[E_MAC] = ui->cvtMacOS;

    resetUI();

    connect( ui->plainTextEdit, SIGNAL( dropFileResult(const QString&, bool,qint64,qint64,const QByteArray&) ), this, SLOT( onDropedFile(const QString&, bool, qint64, qint64 , const QByteArray& )) );
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_loadBtn_clicked()
{
    resetUI();

    QString pickedFileName = QFileDialog::getOpenFileName(this, QStringLiteral("Select a text file to load ") );
    if ( pickedFileName.isNull() || pickedFileName.isEmpty() ) {
        ui->lineEdit->setText( QStringLiteral("User canceled to open file ! ") );
        ui->statusbar->showMessage( QStringLiteral("User canceled to open file ! "), 5000 );
        return;
    }

    QFileInfo fi(pickedFileName);
    if( fi.isFile() ) {
        QString fPath = fi.isSymLink() ? fi.symLinkTarget() : pickedFileName;
        QFile readedFile(fPath);
        if( readedFile.open( QIODevice::ReadOnly) ) {
            QByteArray contentOfFile;
            qint64 fileSize = readedFile.size();
            auto bReadFull = (fileSize <= MB_10);
            if ( bReadFull ) {
                // QByteArray's limit is   2 GB
                contentOfFile = readedFile.readAll();
            } else {
                contentOfFile = readedFile.read(MB_10);
            }

            // read the info and set the content
            QString strContent(contentOfFile);
            ui->plainTextEdit->setPlainText(strContent);

            doBufferAnalyze(fPath, true, (bReadFull ? fileSize : MB_10), fileSize, contentOfFile, false);
        } else {
            ui->lineEdit->setText( QStringLiteral("Can't open the given file ! ") );
            ui->statusbar->showMessage( QStringLiteral("Can't open the given file ! "), 5000 );

            doBufferAnalyze(fPath, false, 0ll, 0ll, QByteArray(), false );
        }
    } else {
        ui->lineEdit->setText( QStringLiteral("Picked target is not a file ! ") );
        ui->statusbar->showMessage( QStringLiteral("Picked target is not a file ! "), 5000 );
        doBufferAnalyze(pickedFileName, false, 0ll, 0ll, QByteArray(), false );
    }
}

void MainWindow::on_saveBtn_clicked()
{
    if ( m_eolType == -1 || m_eolType == E_NOT_DETECHED ) {
        ui->lineEdit->setText(       m_eolType == -1 ? QStringLiteral("No Loaded File ") : QStringLiteral("No <EOL> found in the loaded file ")     );
        ui->statusbar->showMessage( (m_eolType == -1 ? QStringLiteral("No Loaded File ") : QStringLiteral("No <EOL> found in the loaded file ")), 0 );
        return;
    }

    if ( m_bReadState == 0 || m_bReadState == 1 ) {
        ui->lineEdit->setText(        m_bReadState == 0 ? QStringLiteral("Not Read yet :( ")  : QStringLiteral("Partially Read , return :( ") );
        ui->statusbar->showMessage( ( m_bReadState == 0 ? QStringLiteral("Not Read yet  :( ") : QStringLiteral("Partially Read , return :( ") ), 0 );
        return;
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    // Fully read
    //
    ///////////////////////////////////////////////////////////////////////////////
    int newEolType = -1;
    // convert from  original -> new 
    for( int i = 0; i < EOL_CONVERT_CNT; ++i ) {
        if ( m_convertBtnAry[i]->isChecked() ) {
            newEolType = i;
            break;
        }
    }

    if ( newEolType == -1 ) {
        ui->lineEdit->setText( QStringLiteral("Please Picked a new eol flag first ! ") );
        ui->statusbar->showMessage( QStringLiteral("Please Picked a new eol flag first ! "), 0 );
        return;
    }

    if ( newEolType == m_eolType ) {
        ui->lineEdit->setText( QStringLiteral("Same EOL Flag , return ") );
        ui->statusbar->showMessage( QStringLiteral("Same EOL Flag , return "), 0 );
        return;
    }

    // Diff , do convert
    QString errorMsg;
    auto bRet = doConvert( m_loadedFilePath, m_eolType, newEolType, ui->appendEolAtLastFlag->isChecked(),  errorMsg );
    if ( bRet ) {
        QString succMsg = QString("[SUCC] convert %1 -> %2 finished.").arg( m_EOL_NAME[m_eolType] ).arg( m_EOL_NAME[newEolType] );
        ui->lineEdit->setText( succMsg );
        ui->statusbar->showMessage( succMsg, 0 );
    } else {
        QString err = QString("[ERROR] %1").arg( errorMsg );
        ui->lineEdit->setText( err );
        ui->statusbar->showMessage( err, 0 );
    }
}

void MainWindow::on_rescanBtn_clicked()
{
    resetUI();
    ui->plainTextEdit->clear();
}


void MainWindow::onDropedFile(const QString& fPath, bool bReadSucc, qint64 readed, qint64 totally, const QByteArray& buffer)
{
    resetUI();
    doBufferAnalyze(fPath, bReadSucc, readed, totally, buffer, true); // true is a flag of dropEvent
}


void MainWindow::doBufferAnalyze(const QString& fPath, bool bSuccess, qint64 readed, qint64 totally, const QByteArray& buffer, bool isDropFile) 
{
    if ( bSuccess ) {
        m_loadedFilePath = fPath;
        int eolType = E_NOT_DETECHED;
        int eolHybridPre = -1;
        int lineIndex = 0;
        int lineDiffIndex = 0;
        int eolDiffType = 0;

        if ( buffer.isEmpty() ) {
            QString detail = QString("%1 readed <Empty> File ! EOL = %2").arg("[Fully]").arg( m_EOL_NAME[m_eolType==-1 ? E_NOT_DETECHED : m_eolType] );
            ui->lineEdit->setText( detail );
            ui->statusbar->showMessage( detail, 0 );

            m_bReadState = 2; // fully read
            m_eolType = E_NOT_DETECHED; 
            // return;
        } else {
            if ( readed == totally ) {
                m_bReadState = 2; // fully read
            } else {
                m_bReadState = 1; // partially read
            }

            if ( m_modeBtnAry[E_LAZY_MODE]->isChecked() ) {
                // Lazy Mode
                int r_idx = buffer.indexOf(SC_CH_R); // \r
                int n_idx = -1;
                if ( r_idx != -1 ) {
                    n_idx = buffer.indexOf(SC_CH_N, r_idx+1);
                    if ( n_idx == r_idx+1 ) {
                        // \r\n
                        eolType = E_WIN;
                    } else {
                        // \r 's next char is not \n  ==>  \r [Only]
                        eolType = E_MAC;
                    }
                } else {
                    // \r is not found , then search \n
                    n_idx = buffer.indexOf(SC_CH_N);
                    if ( n_idx != -1 ) {
                        // \n is decided
                        eolType = E_UNIX;
                    } 
                    // else {
                    //     neither \r nor \n  has been found
                    //     n_idx == -1
                    //     eolType = E_NOT_DETECHED;
                    // }
                }
            } else {
                // Fully Mode
                int bufSz = static_cast<int>( buffer.size() );
                for ( int i = 0; i < bufSz; ) {
                    auto bNeedIncIdx = true;
                    if ( buffer[i] == '\r' ) {
                        bool hasNextChar = ((i+1) < bufSz);
                        bool bIsNextEscN = hasNextChar && ( buffer[i+1] == '\n' );
                        if ( bIsNextEscN ) {
                            // \r\n
                            if ( eolType == E_NOT_DETECHED ) {
                                lineIndex = 1;
                                eolType = static_cast<int>( E_WIN );

                                i += 2; // skip '\n' forcelly   ==>  jump next char to \n
                                bNeedIncIdx = false;
                            } else {
                                // decided
                                if ( eolType != E_WIN ) {
                                    eolHybridPre = eolType;
                                    eolDiffType = E_WIN;

                                    lineDiffIndex = lineIndex+1;
                                    eolType = E_HYBRID;
                                    
                                    break;
                                } else {
                                    // same
                                    ++lineIndex;

                                    i += 2; // skip '\n' forcelly   ==>  jump next char to \n
                                    bNeedIncIdx = false;
                                }
                            }
                        } else {
                            // such as \r a
                            if ( eolType == E_NOT_DETECHED ) {
                                lineIndex = 1;

                                eolType = static_cast<int>( E_MAC );
                            } else {
                                // decided
                                if ( eolType != E_MAC ) {
                                    eolHybridPre = eolType;
                                    eolDiffType = E_MAC;

                                    lineDiffIndex = lineIndex+1;
                                    eolType = E_HYBRID;
                                    break;
                                } else {
                                    // same
                                    ++lineIndex;
                                }
                            }
                        }
                    } else if ( buffer[i] == '\n' ) {
                        if ( eolType == E_NOT_DETECHED ) {
                            lineIndex = 1;
                            eolType = static_cast<int>( E_UNIX );
                        } else {
                            // decided 
                            if ( eolType != E_UNIX ) {
                                eolHybridPre = eolType;
                                eolDiffType = E_UNIX;

                                lineDiffIndex = lineIndex+1;
                                eolType = E_HYBRID;
                                break;
                            } else {
                                ++lineIndex;
                            }
                        }
                    }

                    if ( bNeedIncIdx ) {
                        ++i;
                    }
                }
            }
        }

        // set Original EOL Mode
          // clear state
        for( int i = 0; i < E_EOL_CNT; ++i ) {
            m_originalBtnAry[i]->setChecked( false );
        }
        if ( eolType >=0   &&  eolType < E_EOL_CNT  ) {
            m_originalBtnAry[eolType]->setChecked( true );
        } else {
            m_originalBtnAry[E_NOT_DETECHED]->setChecked( true );
        }

        ui->saveBtn->setEnabled( true );
        ui->convertFmtBox->setEnabled( true );

          // clear state
        for( int i = 0; i < EOL_CONVERT_CNT; ++i ) {
            m_convertBtnAry[i]->setChecked( false );
        }
        if ( eolType >=0 && eolType < E_HYBRID ) {
            m_convertBtnAry[eolType]->setChecked( true );
        }

        QString lastChInfo = "Last char is not eol. ";
        if ( !buffer.isEmpty() &&  readed == totally ) {
            int sz = buffer.size();
            char lastCh = buffer[sz-1];

            if ( sz > 1 ) {
                char lastButOneCh = buffer[sz-2];
                if ( lastButOneCh == '\r' ) {
                    if ( lastCh == '\n' ) {
                        lastChInfo = "Last 2 chars : \\r\\n => [WIN]";
                        m_bLastEOLFlag = true;
                    } 
                } else {
                    if ( lastCh == '\r' ) {
                        lastChInfo = "Last char is \\r => [MAC]. ";
                        m_bLastEOLFlag = true;
                    } else if ( lastCh == '\n' ) {
                        lastChInfo = "Last char is \\n => [UNIX]. ";
                        m_bLastEOLFlag = true;
                    } 
                }
            } else {
                // sz == 1
                if ( lastCh == '\r' ) {
                    lastChInfo = "Last char is \\r => [MAC]. ";
                    m_bLastEOLFlag = true;
                } else if ( lastCh == '\n' ) {
                    lastChInfo = "Last char is \\n => [UNIX]. ";
                    m_bLastEOLFlag = true;
                } 
            }

            // Update  Del EOL Button State
        } 
        ui->delEOLBtn->setEnabled( m_bLastEOLFlag );
        
        QString hybridDetail;
        if ( eolType == E_HYBRID ) {
            hybridDetail = QString("Pre %1 , line:%2 -> %3").arg( QString("%1 line:%2").arg(m_EOL_NAME[eolHybridPre] ).arg(lineDiffIndex-1) ).arg( lineDiffIndex ).arg( m_EOL_NAME[eolDiffType] );
        }
        // output detail
        QString detail = QString("%1 readed %2 / %3 bytes EOL = [%4] %5 ( %6 )").arg(readed == totally ? "[Fully]" : "[Partially]").arg(readed).arg(totally).arg( m_EOL_NAME[eolType] ).arg( hybridDetail ).arg( lastChInfo );
        ui->lineEdit->setText( detail );
        ui->statusbar->showMessage( detail, 0 );

        m_eolType = eolType;
    } else {
        // Read Failed
        m_eolType = E_NOT_DETECHED;
        QString failedMsg = QString("Failed to read %1 file. ").arg(isDropFile ? "droped" : "loaded");
        ui->lineEdit->setText( failedMsg );
        ui->statusbar->showMessage( failedMsg, 0 );
    }
}



void MainWindow::resetUI()
{
    m_bReadState = 0;
    m_eolType = -1;
    m_loadedFilePath = "";
    m_bLastEOLFlag = false;

    ui->rdOptionBox->setEnabled(true);
    ui->lazyMode->setChecked( true );
    ui->fullMode->setChecked( false );

    // set default as N/A
    ui->originalFmtBox->setEnabled( false );
    for( int i = 0; i < E_EOL_CNT; ++i ) {
        m_originalBtnAry[i]->setAutoExclusive( false ); // Core Core Core
         m_originalBtnAry[i]->setChecked( false );
        m_originalBtnAry[i]->setAutoExclusive( true ); // restore back
    }

    // No default
    ui->convertFmtBox->setEnabled( false );
    ui->saveBtn->setEnabled( false );
    for( int i = 0; i < EOL_CONVERT_CNT; ++i ) {
        m_convertBtnAry[i]->setAutoExclusive( false ); // Core Core Core
         m_convertBtnAry[i]->setChecked( false );
        m_convertBtnAry[i]->setAutoExclusive( true ); // restore back
    }
    ui->appendEolAtLastFlag->setChecked( false );
    ui->delEOLBtn->setEnabled( false );

    // clear log
    ui->lineEdit->clear();
    ui->statusbar->clearMessage();

}



bool MainWindow::doConvert(const QString& fPath, int originalEol, int newEolType, bool bNeedAddeolAtLast, QString& errorMsg)
{
    QFileInfo fInfo(fPath);
    if ( !fInfo.exists() ) {
        errorMsg = QString("The given file \"%1\" is not existed.").arg( fPath );
        return false;
    }

    QFile srcFile(fPath);
    if ( !srcFile.open( QIODevice::ReadOnly ) ) {
        errorMsg = QString("Failed to open the given file \"%1\" .").arg( fPath );
        srcFile.close();
        return false;
    }

    bool bIsLastEOLFlag = false;
    qint64 srcSZ = srcFile.size();
    if ( srcSZ == 0 ) {
        errorMsg = QString("[INFO] Original file is <Empty>. Do Nothing ! ");
        srcFile.close();
        return true;
    } else {
        if ( srcFile.seek(srcSZ-1) ) {
            QByteArray buf1 = srcFile.read( 1 );
            // no matter whether the previous char is \r ( \r\n )   or not  
            bIsLastEOLFlag = ( buf1[0] == SC_CH_R || buf1[0] == SC_CH_N );
        }
        srcFile.seek(0); // rewind to the very beginning read-indicator of the file 
    } 

    if ( originalEol == E_NOT_DETECHED ) {
        if ( bNeedAddeolAtLast ) {
            srcFile.close();
            // append eol 
            QFile appendSrcFile(fPath);
            if ( !appendSrcFile.open( QIODevice::Append  ) ) {
                errorMsg = QString("Failed to open the given for appending file \"%1\" .").arg( fPath );
                appendSrcFile.close();
                return false;
            }

            if ( newEolType == E_UNIX ) {
                appendSrcFile.write(&SC_CH_N, 1);
            } else if ( newEolType == E_WIN ) {
                appendSrcFile.write(&SC_CH_R, 1);
                appendSrcFile.write(&SC_CH_N, 1);
            } else if ( newEolType == E_MAC ) {
                appendSrcFile.write(&SC_CH_R, 1);
            }
            appendSrcFile.flush();
            appendSrcFile.close();
        } else {
            errorMsg = "[INFO] Keep the file's original content !";
        }
        return true;
    }

    QString newFilePath = genTmpFileName( fPath );
    QFileInfo newfi( newFilePath );
    QFile outputFile( newFilePath );
    if ( !outputFile.open( QIODevice::WriteOnly | QIODevice::Truncate ) ) {
        errorMsg = QString("Failed to open the output file. ");
        srcFile.close();
        outputFile.close();
        return false;
    }

    srcFile.seek(0); // rewind to the very beginning read-indicator of the file 
    qint64 charIdx = 0;
    qint64 step = MB_512;
    while ( charIdx < srcSZ )
    {
        QByteArray readBuffer;
        if ( (charIdx + step) <= srcSZ ) {
            charIdx += step;

            readBuffer = srcFile.read( step );
            char lastCh = readBuffer.back();
            if ( lastCh == SC_CH_R ) {
                // check next char is \n or not
                if ( (charIdx+1) <= srcSZ ) {
                    char ch1 = 0;
                    srcFile.read(&ch1, 1);
                    readBuffer.push_back(ch1);

                    ++charIdx;
                }
            } 
        } else {
            qint64 rest = srcSZ - charIdx;
            readBuffer = srcFile.read( rest );

            charIdx += rest;
        }

        int searchPos = 0;
        while( true )
        {
            auto bfoundEOL = true;
            auto bNeedBreak = false;
            QByteArray content;
            int foundPos_R = readBuffer.indexOf( SC_CH_R , searchPos );
            if ( foundPos_R != -1 ) {
                // \r has been already found
                content = readBuffer.mid(searchPos, (foundPos_R - searchPos) );
                if ( foundPos_R == (readBuffer.size()-1) ) {
                    // is the last char ?
                    searchPos = foundPos_R + 1;
                } else {
                    if ( readBuffer[foundPos_R + 1] == SC_CH_N ) {
                        // \r + \n
                        searchPos = (foundPos_R + 2);
                    } else {
                        searchPos = (foundPos_R + 1);
                    }
                }
            } else {
                // not     found \r
                // try to  found \n
                int foundPos_N = readBuffer.indexOf( SC_CH_N , searchPos );
                if ( foundPos_N == -1 ) {
                    content = readBuffer.mid(searchPos);
                    bfoundEOL = false;
                    bNeedBreak = true; // to break
                } else {
                    content = readBuffer.mid(searchPos, (foundPos_N-searchPos) );
                    searchPos = foundPos_N + 1;
                }
            }

            if ( !content.isEmpty() ) {
                outputFile.write( content );
            }

            if ( bfoundEOL ) {
                if ( newEolType == E_UNIX ) {
                    outputFile.write(&SC_CH_N, 1);
                } else if ( newEolType == E_WIN ) {
                    outputFile.write(&SC_CH_R, 1);
                    outputFile.write(&SC_CH_N, 1);
                } else if ( newEolType == E_MAC ) {
                    outputFile.write(&SC_CH_R, 1);
                }
            }

            if ( bNeedBreak ) {
                break;
            }
        }
    }

    if ( bNeedAddeolAtLast && !bIsLastEOLFlag ) {
        if ( newEolType == E_UNIX ) {
            outputFile.write(&SC_CH_N, 1);
        } else if ( newEolType == E_WIN ) {
            outputFile.write(&SC_CH_R, 1);
            outputFile.write(&SC_CH_N, 1);
        } else if ( newEolType == E_MAC ) {
            outputFile.write(&SC_CH_R, 1);
        }
    }

    outputFile.flush();
    outputFile.close();
    srcFile.close();

    QDir fileDir( fInfo.absolutePath() );
    return      fileDir.remove( fInfo.fileName()  )  // delete original file
            &&  fileDir.rename( newfi.fileName(),  fInfo.fileName()  ); // rename tmp-filename -> original file name


}



QString MainWindow::genTmpFileName(const QString& srcAbsPath)
{
    QFileInfo fInfo(srcAbsPath);

    QFileInfo newfi;
    QString newFilePath;
    do {
        quint32 ranNum = QRandomGenerator::system()->generate();
        newFilePath = QString("%1/%2_tmp%3").arg( fInfo.absolutePath() ).arg( fInfo.baseName() ).arg( ranNum );
        QString cmpSuffixPath = fInfo.completeSuffix();
        if ( !cmpSuffixPath.isEmpty() ) {
            newFilePath += QChar('.');
            newFilePath += cmpSuffixPath;
        } else if ( srcAbsPath.endsWith( QChar('.') ) ) {
            newFilePath += QChar('.');
        } 

        newfi.setFile( newFilePath );
    } while( newfi.exists() );

    // qDebug() << "newFilePath = " << newFilePath;

    return newFilePath;
}


bool MainWindow::doDeleteEndEOL(QString& errorMsg)
{
    QFile srcFile(m_loadedFilePath);
    if( !srcFile.open( QIODevice::ReadOnly) ) {
        errorMsg = QString("Failed to open the given file \"%1\" .").arg( m_loadedFilePath );
        srcFile.close();
        return false;
    }

    QString newFileName = genTmpFileName(m_loadedFilePath);
    QFile outputFile( newFileName );
    if ( !outputFile.open( QIODevice::WriteOnly | QIODevice::Truncate ) ) {
        errorMsg = QString("Failed to open the output file '%1' . ").arg( newFileName) ;
        srcFile.close();
        outputFile.close();
        return false;
    }

    QByteArray contentPart;
    qint64 fileSize  = srcFile.size();
    qint64 partCnt   = fileSize / MB_10;
    qint64 restBytes = fileSize % MB_10;

    for ( qint64 i = 0; i < partCnt; ++i ) {
        contentPart = srcFile.read(MB_10);
        if ( restBytes == 0 &&  (i==partCnt-1) ) {
            if (         contentPart[contentPart.size()-2] == SC_CH_R 
                    &&   contentPart[contentPart.size()-1] == SC_CH_N )
            {
                // \r \n
                errorMsg = "\\r\\n";
                contentPart.truncate( static_cast<int>(MB_10 - 2) );
            } else {
                // must be  \r  or \n
                errorMsg = (contentPart[contentPart.size()-1] == SC_CH_R ? "\\r" : "\\n");
                contentPart.truncate( static_cast<int>(MB_10 - 1) );
            }
            outputFile.write( contentPart );
        } else {
            // write as normal
            outputFile.write( contentPart );
        }
    }

    if ( restBytes > 0 ) {
        contentPart = srcFile.read(restBytes);
        if (        contentPart.size() >= 2 
                &&  contentPart[contentPart.size()-2] == SC_CH_R 
                &&  contentPart[contentPart.size()-1] == SC_CH_N )
        {
            // \r \n
            errorMsg = "\\r\\n";
            contentPart.truncate( static_cast<int>(restBytes - 2) );
        } else {
            // must be  \r  or \n
            errorMsg = (contentPart[contentPart.size()-1] == SC_CH_R ? "\\r" : "\\n");
            contentPart.truncate( static_cast<int>(restBytes - 1) );
        }

        if ( !contentPart.isEmpty() ) {
            outputFile.write( contentPart );
        }
    }

    srcFile.close();
    outputFile.flush();
    outputFile.close();

    QFileInfo fInfo( m_loadedFilePath );
    QFileInfo newfi( newFileName );

    QDir fileDir( fInfo.absolutePath() );

    return      fileDir.remove( fInfo.fileName()  )  // delete original file
            &&  fileDir.rename( newfi.fileName(),  fInfo.fileName()  ); // rename tmp-filename -> original file name

}


void MainWindow::on_delEOLBtn_clicked()
{
    QString errorMsg;
    auto bret = doDeleteEndEOL(errorMsg);
    if ( bret ) {
        QString info = QString("[SUCC] Delete Last EOL %1 Done").arg(errorMsg);
        ui->lineEdit->setText( info );
        ui->statusbar->showMessage( info, 0 );
    } else {
        ui->lineEdit->setText( errorMsg );
        ui->statusbar->showMessage( errorMsg, 0 );
    }

}



