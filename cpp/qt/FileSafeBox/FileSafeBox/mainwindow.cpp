#include <QDebug>
#include <QSize>
#include <QTimer>
#include <QTime>
#include <QResizeEvent>
// #include <QElapsedTimer> 
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>

#include <limits>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyWidget.h"
#include "EncdecUtil.h"
#include "FileencdecUtil.h"


namespace testcase {
    static const bool DEBUG_FLAG = false;
}

// static 
// const int  QMainWindow::SC_TOTAL_CNT = 4;

/*
       0(lt)                  3(rt)
        -----------------------
        |                     |
        |                     |
        |                     |
        |                     |
        -----------------------
       1(lb)                  2(rb)


       ( 0 --> 1 --> 2 --> 3 )
*/
MainWindow::MainWindow(QWidget *parent /* = nullptr */) :
      QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_lt(nullptr)
    , m_rt(nullptr)
    , m_lb(nullptr)
    , m_rb(nullptr)
    , m_timer(nullptr)
    , m_widgetAry{ nullptr, nullptr, nullptr, nullptr }
    , m_colorStyleAry{
        QStringLiteral("QWidget { background : red; }"),
        QStringLiteral("QWidget { background : green; }"),
        QStringLiteral("QWidget { background : blue; }"),
        QStringLiteral("QWidget { background : yellow; }")
      }
    , m_openedFileName()
    , m_encryptedFileBuf()
    , m_hoverDuring( 2500 )
    , m_slotDoneDuring( 800 )
    , m_passwordshowDuring( 2250 )
    , m_infoMsgShowDuring( 3000 )
    , m_attemptCntMax(3)
    , m_bdOrderAry{ 0u, 2u , 3u, 1u }
    , m_currentTargetIdx(0)
    , m_bBDUnlocked( false )
    , m_bNextEnable( false )
    , m_attemptCnt(0)
    , m_btnState(0)
{
    ui->setupUi(this);

    m_encryptedFileBuf.clear();

    // left top
    m_lt = new MyWidget(this, 0);
    // m_lt->setStyleSheet("QWidget { background : red; }");
    connect(m_lt, &MyWidget::onMouseEnter, this, &MainWindow::onMouseEnterWidget);
    connect(m_lt, &MyWidget::onMouseLeave, this, &MainWindow::onMouseLeaveWidget);

    // right top
    m_rt = new MyWidget(this, 3);
    // m_rt->setStyleSheet("QWidget { background : green; }");
    connect(m_rt, &MyWidget::onMouseEnter, this, &MainWindow::onMouseEnterWidget);
    connect(m_rt, &MyWidget::onMouseLeave, this, &MainWindow::onMouseLeaveWidget);

    // left bottom
    m_lb = new MyWidget(this, 1);
    // m_lb->setStyleSheet("QWidget { background : blue; }");
    connect(m_lb, &MyWidget::onMouseEnter, this, &MainWindow::onMouseEnterWidget);
    connect(m_lb, &MyWidget::onMouseLeave, this, &MainWindow::onMouseLeaveWidget);

    // right bottom
    m_rb = new MyWidget(this, 2);
    // m_rb->setStyleSheet("QWidget { background : yellow; }");
    connect(m_rb, &MyWidget::onMouseEnter, this, &MainWindow::onMouseEnterWidget);
    connect(m_rb, &MyWidget::onMouseLeave, this, &MainWindow::onMouseLeaveWidget);

    setPosAndSize( this->size() );

    m_widgetAry[0] = m_lt; 
    m_widgetAry[1] = m_lb; 
    m_widgetAry[2] = m_rb; 
    m_widgetAry[3] = m_rt; 

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval( m_hoverDuring );
    m_timer->stop();
    connect(m_timer, &QTimer::timeout,  this, &MainWindow::onTimeOut );

    closeBD();
    ui->encdecButton->setEnabled( false );

    // testEncDec();
    // testEncDec2();

}

MainWindow::~MainWindow()
{
    delete ui;
}


// virtual
void   MainWindow::resizeEvent(QResizeEvent * event) // Q_DECL_OVERRIDE;
{
    QMainWindow::resizeEvent(event);

    setPosAndSize(event->size() );
}


void MainWindow::setPosAndSize(const QSize& sz)
{
    static const QSize fixedsz(15,15);

    m_lt->setGeometry(0,0, fixedsz.width(),fixedsz.height() );
    m_rt->setGeometry( sz.width() - fixedsz.width(),0, fixedsz.width(), fixedsz.height() );
    m_lb->setGeometry( 0,sz.height() - fixedsz.width(), fixedsz.width(),fixedsz.height() );
    m_rb->setGeometry( sz.width() - fixedsz.width(),sz.height() - fixedsz.width(), fixedsz.width(),fixedsz.width());
}


void MainWindow::onMouseEnterWidget(MyWidget* w)
{
    if ( m_bBDUnlocked    &&   w!=nullptr ) {
        auto tag = w->getTag();
        if ( tag == m_bdOrderAry[m_currentTargetIdx] ) {
            if ( testcase::DEBUG_FLAG ) {
                qDebug() << "Enter Widget Hit ... " << tag;
            }
            m_timer->start();
        } else {
            if ( testcase::DEBUG_FLAG ) {
                qDebug() << "Back to 0";
            }
            m_currentTargetIdx = 0;
        }
        m_bNextEnable  = false;
    }
}

void MainWindow::onMouseLeaveWidget(MyWidget* w)
{
    if( m_bBDUnlocked   &&  w != nullptr) {
        auto tag = w->getTag();
        if (  m_bNextEnable   &&    tag == m_bdOrderAry[m_currentTargetIdx]  ) {
            if ( testcase::DEBUG_FLAG ) {
                qDebug() << "Leave Widget Hit ... " << tag;
                m_widgetAry[tag]->setStyleSheet( m_colorStyleAry[tag] );
            }
            
            ++m_currentTargetIdx;
            if ( m_currentTargetIdx >= WIDGETS_CNT ) {
                onBDTakeAction();
            }
        }
    }
}


void MainWindow::onTimeOut()
{
    if ( testcase::DEBUG_FLAG ) {
        ui->statusBar->showMessage( QStringLiteral("     %1 time elapsed").arg( m_bdOrderAry[m_currentTargetIdx] ) , m_slotDoneDuring);
    }
    m_bNextEnable = true;
}


void MainWindow::showBDWidgets(bool show)
{
    if ( show ) {
        m_lt->show(); 
        m_rt->show(); 
        m_lb->show(); 
        m_rb->show();
    } else {
        m_lt->hide(); 
        m_rt->hide(); 
        m_lb->hide(); 
        m_rb->hide();
    }

}


void MainWindow::onBDTakeAction()
{
    FileEncDecUtil fu;
    fu.setFileContent( m_encryptedFileBuf );

    QByteArray decContentBuf;
    QByteArray bufPwd;
    FileEncDecUtil::collectedCoreData  decKeyInfo;
    int decRet = fu.decFileContent(&decContentBuf, bufPwd, &decKeyInfo);
    if ( decRet == 2 ) {
        // incorrect password , Show Real Password
        ui->statusBar->showMessage( QStringLiteral("[INFO] Pwd : \"%1\"").arg( QString(decKeyInfo.password) ),   m_passwordshowDuring);
    } else {
        ui->statusBar->showMessage( QStringLiteral("[WARNING] Back door doesn't work . "),   m_infoMsgShowDuring );
    }
    
    closeBD();
}

void MainWindow::closeBD()
{
    m_bBDUnlocked = false;
    showBDWidgets(false);

    ui->openFileButton->setEnabled( true );
    ui->encdecButton->setEnabled( true );

    ui->passwordInput->setReadOnly( false );
    ui->fileContent->setReadOnly( false ); 
}


void MainWindow::lauchBD()
{
    m_bBDUnlocked = true;
    m_currentTargetIdx = 0;
    m_bNextEnable = false;
    m_timer->stop();

    showBDWidgets(true);


    ui->openFileButton->setEnabled( false );
    ui->encdecButton->setEnabled( false );

    ui->passwordInput->setReadOnly( true );
    setFileContentLockState(true, QString() );
}




void MainWindow::testEncDec()
{
    const int ARY_SZ = 256;

    char chMin = std::numeric_limits<char>::min();
    char chMax = std::numeric_limits<char>::max();
    char chAry[ARY_SZ] = { 0 };
    char chAryConvert[ARY_SZ] = { 0 };
    auto canrevertBack = true;

    int idx = 0;
    char ch = chMin;
    while ( ch != chMax ) {
        chAry[idx] = ch;
        ++idx;
        ++ch;
    }
    qDebug() << "1. idx = " << idx;
    chAry[idx] = chMax;

    ///////////////////////////////////////////////////////////////////////////
    //
    // enc 1
    idx = 0;
    for( idx = 0; idx < ARY_SZ; ++idx ) {
        chAryConvert[idx] = EncryptDecryptUtil::enc1ch(1, chAry[idx] );
    }

    // dec 1
    idx = 0;
    canrevertBack = true;
    for( idx = 0; idx < ARY_SZ; ++idx ) {
        if( EncryptDecryptUtil::dec1ch(1,  chAryConvert[idx] ) != chAry[idx] ) {
            canrevertBack = false;
            break;
        }
    }

    if ( !canrevertBack ) {
        qDebug() << "enc1 -> dec1 failed : " <<  idx << " , 0x"
                 << static_cast<int>( 0xFF & chAryConvert[idx] )
                 << " -> 0x" 
                 << static_cast<int>( 0xFF & chAry[idx] );
    } else {
        qDebug() << "enc1  OK";
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    // enc 2
    idx = 0;
    for( idx = 0; idx < ARY_SZ; ++idx ) {
        chAryConvert[idx] = EncryptDecryptUtil::enc1ch(2, chAry[idx] );
    }

    // dec 2
    idx = 0;
    canrevertBack = true;
    for( idx = 0; idx < ARY_SZ; ++idx ) {
        if( EncryptDecryptUtil::dec1ch(2, chAryConvert[idx] ) != chAry[idx] ) {
            canrevertBack = false;
            break;
        }
    }

    if ( !canrevertBack ) {
        qDebug() << "enc2 -> dec2 failed : " << idx << " , 0x" 
                 << static_cast<int>( 0xFF & chAryConvert[idx] )
                 << " -> 0x" 
                 << static_cast<int>( 0xFF & chAry[idx] );
    } else {
        qDebug() << "enc2  OK";
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    // enc 3
    idx = 0;
    for( idx = 0; idx < ARY_SZ; ++idx ) {
        chAryConvert[idx] = EncryptDecryptUtil::enc1ch(3, chAry[idx] );
    }

    // dec 3
    idx = 0;
    canrevertBack = true;
    for( idx = 0; idx < ARY_SZ; ++idx ) {
        if( EncryptDecryptUtil::dec1ch(3, chAryConvert[idx] ) != chAry[idx] ) {
            canrevertBack = false;
            break;
        }
    }

    if ( !canrevertBack ) {
        qDebug() << "enc3 -> dec3 failed : " << idx << " , 0x" 
                 << static_cast<int>( 0xFF & chAryConvert[idx] )
                 << " -> 0x" 
                 << static_cast<int>( 0xFF & chAry[idx] );
    } else {
        qDebug() << "enc3  OK";
    }

    ///////////////////////////////////////////////////////////////////////////
    //
    // enc 4
    idx = 0;
    for( idx = 0; idx < ARY_SZ; ++idx ) {
        chAryConvert[idx] = EncryptDecryptUtil::enc1ch(4, chAry[idx] );
    }

    // dec 4
    idx = 0;
    canrevertBack = true;
    for( idx = 0; idx < ARY_SZ; ++idx ) {
        if( EncryptDecryptUtil::dec1ch(4, chAryConvert[idx] ) != chAry[idx] ) {
            canrevertBack = false;
            break;
        }
    }

    if ( !canrevertBack ) {
        qDebug() << "enc4 -> dec4 failed : " << idx << " , 0x" 
                 << static_cast<int>( 0xFF & chAryConvert[idx] )
                 << " -> 0x" 
                 << static_cast<int>( 0xFF & chAry[idx] );
    } else {
        qDebug() << "enc4 OK";
    }
}


void MainWindow::testEncDec2()
{
    const int BYTE_BITS   = 8;
    const int BYTE_BITS_2 = BYTE_BITS * 2;
    const int ARY_SZ = 256;

    char chMin = std::numeric_limits<char>::min();
    char chMax = std::numeric_limits<char>::max();

    char chAry1[ARY_SZ] = { 0 };
    char chAry2[ARY_SZ] = { 0 };

    int bit16[BYTE_BITS_2] = { 0 };
    int bit16_[BYTE_BITS_2] = { 0 };

    // auto canRevertBack = true;

    int i = 0;
    int j = 0;
    char ch = chMin;
    while ( ch != chMax ) {
        chAry1[i] = ch;
        chAry2[i] = ch;
        ++i;
        ++ch;
    }
    // qDebug() << "1. idx = " << idx;
    chAry1[i] = chMax;
    chAry2[i] = chMax;

    char out1 = 0;
    char out2 = 0;
    auto isMatched = true;
    for( i = 0; i < ARY_SZ; ++i ) {
        getBitAry( chAry1[i], bit16 );
        for( j = 0; j < ARY_SZ; ++j ) {
            getBitAry2( chAry1[i], chAry2[j],  bit16);
            EncryptDecryptUtil::enc2ch(4, chAry1[i], chAry2[j], &out1, &out2);
            getBitAry2(out1, out2, bit16_ ); 

            
            for( int k = 0; k < BYTE_BITS_2; ++k ) {
                if ( k == 0  || k == BYTE_BITS_2-1 ) {
                    if ( bit16[k] != bit16_[BYTE_BITS_2-k-1] ) {
                        isMatched = false;
                        break;
                    }
                } else {
                    if ( bit16[k] != bit16_[k] ) {
                        isMatched = false;
                        break;
                    }
                }
            }

            if ( !isMatched ) {
                break;
            } else {
                EncryptDecryptUtil::dec2ch(4, out1, out2, &out1, &out2);
                if ( !(       (out1 == chAry1[i])
                         &&   (out2 == chAry2[j] ) )    )
                {
                    isMatched = false;
                    break;
                }
            }
        }

        if ( !isMatched ) {
            break;
        }
    }

    if ( !isMatched ) {
        qDebug() << "Not Matched : " << endl
                 << "chAry1[" << i << "] = "  << getStrForAry16(bit16) << endl
                 << "chAry2[" << i << "] = "  << getStrForAry16(bit16_) << endl;
    } else {
        qDebug() << "All Matched :) ";
    }


    /*
    idx = 0;
    auto canrevertBack = true;
    for( idx = 0; idx < ARY_SZ; ++idx ) {
        if( EncryptDecryptUtil::dec4( chAryConvert[idx] ) != chAry[idx] ) {
            canrevertBack = false;
            break;
        }
    }

    if ( !canrevertBack ) {
        qDebug() << "enc4 -> dec4 failed : " <<  idx << " , 0x"
                 << static_cast<int>( 0xFF & chAryConvert[idx] )
                 << " -> 0x" 
                 << static_cast<int>( 0xFF & chAry[idx] );
    } else {
        qDebug() << "enc4 <--> dec4 OK";
    }
    */
}



void MainWindow::getBitAry(char ch, int* bitAry8)
{
    const int bytePerBit = 8;
    for( int i = 0; i < bytePerBit; ++i ) {
        bitAry8[i] = (ch >> ( bytePerBit - (i+1) ) & 0x1 );
    }
}


void MainWindow::getBitAry2(char ch1, char ch2, int* bitAry16)
{
    const int bytePerBit = 8;
    // const int bytePerBit2 = bytePerBit * 2;
    int i = 0;
    for( i = 0; i < bytePerBit; ++i ) {
        bitAry16[i] = (ch1 >> ( bytePerBit - (i+1) ) & 0x1 );
    }

    for( i = 0; i < bytePerBit; ++i ) {
        bitAry16[bytePerBit + i] = (ch2 >> ( bytePerBit - (i+1) ) & 0x1 );
    }
}

QString MainWindow::getStrForAry8(int* ary)
{
    QString strRet;
    const int bytePerBit = 8;
    for( int  i = 0; i < bytePerBit; ++i ) {
        strRet += static_cast<char>( '0' + ary[i] );
    }

    return strRet;
}

QString MainWindow::getStrForAry16(int* ary)
{
    QString strRet;
    const int bytePerBit = 16;
    for( int  i = 0; i < bytePerBit; ++i ) {
        strRet += static_cast<char>( '0' + ary[i] );
    }

    return strRet;
}

void MainWindow::on_openFileButton_clicked()
{
    m_btnState = 0;
    ui->filenameDisplay->setText("");
    m_openedFileName = "";
    ui->fileContent->setPlainText("");

    QString strfileToOpen = QFileDialog::getOpenFileName(this, QStringLiteral("Pick a file to open") );
    qDebug() << "File to open : " << strfileToOpen;
    if ( strfileToOpen.isNull() ) {
        return;
    }

    QFile fToOpen( strfileToOpen );
    auto bOpenSuccess = fToOpen.open( QIODevice::ReadOnly );
    if ( !bOpenSuccess ) {
        QMessageBox::warning(this, QStringLiteral("Open File Failed"), QStringLiteral("Can't open this file") );
        return;
    }

    ui->filenameDisplay->setText( strfileToOpen );
    m_openedFileName = strfileToOpen;
    QByteArray fileBuffer = fToOpen.readAll();
    fToOpen.close();

    FileEncDecUtil fu;
    int ftype = fu.setFileContent( fileBuffer );
    if ( ftype == 0 ) { // normal file
        m_attemptCnt = 0;
        setBtnState(true);

        setFileContentLockState(false , QString(fileBuffer) );
        ui->statusBar->showMessage( QStringLiteral("Normal File"), m_infoMsgShowDuring ); 
    } else { // ftype == 1   ,  already Encrypted file ( need password )
        setBtnState(false);

        setFileContentLockState(true , QString() );
        m_encryptedFileBuf = fileBuffer;
        ui->statusBar->showMessage( QStringLiteral("Locked File"), m_infoMsgShowDuring ); 
    }
}





void MainWindow::on_encdecButton_clicked()
{
    if ( m_btnState == 1 ) {
        //
        // Normal File : 
        //      < !!! Do Encrypt !!! > by password
        //
        auto content = ui->fileContent->toPlainText();
        if ( content.isNull() || content.isEmpty() ) {
            ui->statusBar->showMessage( QStringLiteral("Content is Blank"), m_infoMsgShowDuring );
            return;
        }

        QString strPwd = ui->passwordInput->text();
        if ( strPwd.isNull() || strPwd.isEmpty() ) {
            ui->statusBar->showMessage( QStringLiteral("Password can't be Blank"), m_infoMsgShowDuring );
            return;
        }

        QByteArray contentBa;
        contentBa.append(content);
        QByteArray bufPwd;
        bufPwd.append( strPwd );
        
        FileEncDecUtil fu;
        fu.setFileContent( contentBa );

        // FileEncDecUtil::collectedCoreData  encKeyInfo;
        // FileEncDecUtil::collectedCoreData* pEncKeyInfo = &encKeyInfo;
        FileEncDecUtil::collectedCoreData* pEncKeyInfo = nullptr;

        QByteArray encryptedBuf;
        int encRet = fu.encFileContent( &encryptedBuf, bufPwd, pEncKeyInfo);
        if ( encRet == 1 ) {
            QFile fToOpen( m_openedFileName );
            if ( fToOpen.open( QIODevice::ReadOnly )  ) {
                fToOpen.close();

                QFile fToWrite( m_openedFileName );
                if ( fToWrite.open( QIODevice::WriteOnly ) ) {
                    auto nWrittenCnt = fToWrite.write(encryptedBuf);
                    if ( static_cast<int>(nWrittenCnt) == encryptedBuf.size() ) {
                        fToWrite.flush();
                        setFileContentLockState( true , QString() );
                        setBtnState( false );

                        ui->passwordInput->setText("");
                        ui->statusBar->showMessage( QStringLiteral("Encrypted Success"), m_infoMsgShowDuring);
                    } else {
                        ui->statusBar->showMessage( QStringLiteral("Write Failed"), m_infoMsgShowDuring);
                    }
                    fToWrite.close();
                } else {
                    ui->statusBar->showMessage( QStringLiteral("Can't open file for write"), m_infoMsgShowDuring);
                }
            } else {
                ui->statusBar->showMessage( QStringLiteral("Can't open file"), m_infoMsgShowDuring);
            }
        } else {
            ui->statusBar->showMessage( QStringLiteral("Encrypt Buff Failed"), m_infoMsgShowDuring);
        }

    } else if ( m_btnState == 2 ) {
        //
        // Encrypted  File : 
        //      < !!! Do Decrypt !!! > by password
        //

        QString strPwd = ui->passwordInput->text();
        if ( strPwd.isNull() || strPwd.isEmpty() ) {
            ui->statusBar->showMessage( QStringLiteral("Password can't be Blank"), m_infoMsgShowDuring );
            return;
        }

        QByteArray bufPwd;
        bufPwd.append( strPwd );

        FileEncDecUtil fu;
        int fType = fu.setFileContent( m_encryptedFileBuf );
        if ( fType == 1 ) {
            QByteArray decContentBuf;

            // FileEncDecUtil::collectedCoreData  decKeyInfo;
            // FileEncDecUtil::collectedCoreData* pDecKeyInfo = &decKeyInfo;
            FileEncDecUtil::collectedCoreData* pDecKeyInfo = nullptr;

            int decRet = fu.decFileContent(&decContentBuf, bufPwd, pDecKeyInfo);
            if ( decRet == 0 ) {
                ui->statusBar->showMessage( QStringLiteral("Already decrypted File Buff"), m_infoMsgShowDuring);
            } else if ( decRet == 1 ) {
                //////////////////////////////////////////////////////////////////////////////////// 
                //
                // Success :  password is correct
                //
                //////////////////////////////////////////////////////////////////////////////////// 
                setFileContentLockState( false , QString(decContentBuf) );

                m_encryptedFileBuf.clear();
                ui->passwordInput->setText("");

                // 
                ui->encdecButton->setEnabled( false );
                ui->encdecButton->setText("Decrypted Done");

                ui->statusBar->showMessage( QStringLiteral("Decrypt Successful :)"), m_infoMsgShowDuring);
            } else if ( decRet == 2 ) {
                ++m_attemptCnt;
                QMessageBox::warning(this, QStringLiteral("ERROR"), QStringLiteral("Password incorrected") );
                if ( m_attemptCnt >= m_attemptCntMax ) {
                    // Open Back Door
                    lauchBD();
                }
            } else if ( decRet == 3 ) {
                ui->statusBar->showMessage( QStringLiteral("Decrpted meet an unexpected Error :)"), m_infoMsgShowDuring);
            }
        } else {
            ui->statusBar->showMessage( QStringLiteral("Unknown Encrypted File Buff"), m_infoMsgShowDuring);
        }
    } else {
        ui->statusBar->showMessage( QStringLiteral("btnState == 0 , Do Nothing"), m_infoMsgShowDuring);
    }

}






void MainWindow::testCaseEncAndDecFile()
{
    auto content = ui->fileContent->toPlainText();
    auto strPwd = ui->passwordInput->text();

    QByteArray fcontent;
    fcontent += content;
    QByteArray pwdbuf;
    pwdbuf += strPwd;

    FileEncDecUtil fu;
    int ftype = fu.setFileContent( fcontent );
    qDebug() << "0. ftype = " << ftype;

    QByteArray outBuf1;
    FileEncDecUtil::collectedCoreData keyDataEnc;
    int encRet = fu.encFileContent(&outBuf1, pwdbuf, &keyDataEnc);  
    qDebug() << "encRet = " << encRet;


    ftype = fu.setFileContent( outBuf1 );
    qDebug() << "1. ftype = " << ftype;

    QByteArray outBuf2;
    FileEncDecUtil::collectedCoreData keyDataDec;
    int decRet = fu.decFileContent(&outBuf2, pwdbuf, &keyDataDec);
    qDebug() << "decRet = " << decRet;

    if ( keyDataEnc.headerInfo == keyDataDec.headerInfo ) {
        qDebug() << "enc.header == dec.header";
    } else {
        qDebug() << "enc.header != dec.header";
    }

    if ( keyDataEnc.metaVec == keyDataDec.metaVec ) {
        qDebug() << "enc.metaVec == dec.metaVec";
    } else {
        qDebug() << "enc.metaVec != dec.metaVec";
    }

    if ( keyDataEnc.contentPwdPuzzle_enc == keyDataDec.contentPwdPuzzle_enc ) {
        qDebug() << "enc.contentPwdPuzzle_enc == dec.contentPwdPuzzle_enc";
    } else {
        qDebug() << "enc.contentPwdPuzzle_enc != dec.contentPwdPuzzle_enc";
    }

    if ( keyDataEnc.contentPwdPuzzle_ori == keyDataDec.contentPwdPuzzle_ori ) {
        qDebug() << "enc.contentPwdPuzzle_ori == dec.contentPwdPuzzle_ori";
    } else {
        qDebug() << "enc.contentPwdPuzzle_ori != dec.contentPwdPuzzle_ori";
        /*
        qDebug() << "enc.size = " << keyDataEnc.contentPwdPuzzle_ori.size() << ", dec.size = " << keyDataDec.contentPwdPuzzle_ori.size();
        int minSz = keyDataEnc.contentPwdPuzzle_ori.size() < keyDataDec.contentPwdPuzzle_ori.size() ? keyDataEnc.contentPwdPuzzle_ori.size() :  keyDataDec.contentPwdPuzzle_ori.size();
        for( int i = 0; i < minSz; ++i ) {
            auto ch1 = keyDataEnc.contentPwdPuzzle_ori[i];
            auto ch2 = keyDataDec.contentPwdPuzzle_ori[i];
            qDebug() << (i+1) << ". " << (ch1 == ch2 ? " == " : "!=" );
        }
        */
    }


    if ( fcontent == outBuf2  ) {
        qDebug() << "enc / dec   : Restore Same";
    } else {
        qDebug() << "enc / dec   : !!!! ERROR !!!!";
    }
}


void MainWindow::setFileContentLockState(bool locked, const QString& filecontent)
{
    if ( !locked ) {

        ui->fileContent->setReadOnly( false ); 
        ui->fileContent->setPlainText( filecontent );

    } else {
        ui->fileContent->setReadOnly( true ); 
        ui->fileContent->setPlainText( 
QStringLiteral(
R"(================================================
   File has been encrypted !!! 
================================================)" ) );

    }
}


void MainWindow::setBtnState(bool isEncryptState)
{
    ui->encdecButton->setEnabled( true );
    if ( isEncryptState ) {
        m_btnState = 1;
        ui->encdecButton->setText("Encrypt");
    } else {
        m_btnState = 2;
        ui->encdecButton->setText("Decrypt");
    }
}

