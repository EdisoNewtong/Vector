#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
// #include <QByteArray>

#include <QTextCharFormat>
#include <QTextImageFormat>
#include <QTextBlock>
#include <QFile>
#include <QFileDialog>

namespace charRange
{
    static const QChar sc_num0('0');
    static const QChar sc_num9('9');
    static const QChar sc_numA('A');
    static const QChar sc_numF('F');
    static const QChar sc_numa('a');
    static const QChar sc_numf('f');
    static const QChar sc_space(' ');
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Every group format must be a 3 letter group : 'xx ' which x must in the range [0-9A-F] 
// and then add a <space> after every 2 Hex numbers
// 
////////////////////////////////////////////////////////////////////////////////////////////////////
static const int modGrp = 3;

MainWindow::MainWindow(QWidget *parent) :
      QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_BufferArray()
    , m_charMapString()
    , m_BufferChangedFlag(false)
{
    ui->setupUi(this);

    this->setWindowTitle( tr("Binary Editor") );

    connect( ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(onBinEditorTextCursorChanged()) );
    connect( ui->textEdit, SIGNAL(textChanged()), this, SLOT(onBinEditorTextContentChanged()) );
    connect( ui->textEdit, SIGNAL(invalidInputKey(const QString&, bool)), this, SLOT(onInvalidKeyInput(const QString&, bool)) );

    m_BufferArray.clear();
    ui->plainTextEdit->setPlaceholderText( tr("e.g. : 3f, 0xf, 0XF, 0x3F") );

    //
    // fill map
    //
    QByteArray ba;
    m_charMapString.clear();
    for ( int i = 0; i < sc_ArySize; ++i ) {
        ba.clear();
        ba.setNum(i,16);
        if ( ba.size() > 2 ) {
            ba = ba.right(2);
        }

        ba = ba.rightJustified(2, '0');
        ba = ba.toUpper();
        QString displaystr(ba);
        m_charMapString.insert( i, displaystr);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_clearContentBtn_clicked()
{
    ui->textEdit->clear();
}

//
// Open/Insert File in binary mode
//
void MainWindow::on_openFileBtn_clicked()
{
    QString errMsg;
    auto bFlag = travelsalBinaryEditor(errMsg);
    if ( !bFlag ) {
        reportOperationResult( QString("Buffer is Invalid : '%1', Please correct the Buffer Content first !!").arg(errMsg), true );
        return;
    }

    auto curPos = ui->textEdit->textCursor().position();
    auto insertPosType = checkInsertPositionIsValid();
    if ( insertPosType == 0 ) {
        reportOperationResult( QString("Insert Position '%1' is not allowed! Valid position is begin / end / cursor index % 3 == 0").arg(curPos), true );
        return;
    }

    // QString	QFileDialog::getOpenFileName(QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0)
    QString filename = QFileDialog::getOpenFileName( this, tr("Select a file to Open/Insert") );
    if ( filename.isEmpty() ) {
        return;
    }
    // qDebug() << "Opened filename = " << filename;

    QFile file(filename);
    if ( !file.open(QIODevice::ReadOnly ) ) {
        reportOperationResult( QString("Can't Open/Insert File : %1").arg(filename), true );
        return;
    }

    // Open File Successful
    qint64 fileSz = file.size();
    // qDebug() << "fileBuffer.size() => " << sz;
    if ( fileSz == 0 ) {
        reportOperationResult( QString("File : <%1>    contain   0 byte content . Nothing to insert !").arg(filename), false );
        return;
    }

    // fileSz != 0
    QByteArray fileBuffer = file.read(fileSz);
    if ( fileBuffer.isEmpty() ) {
        reportOperationResult( QString("File : <%1>    read   0 byte in the content . Nothing to insert !").arg(filename), true );
        return;
    }
    
    if ( fileSz != fileBuffer.size() ) {
        reportOperationResult( QString("File : <%1>    read   %2 / %3 byte(s) when reading . ").arg(filename).arg(fileBuffer.size()).arg(fileSz), true );
        return;
    }

    QString insertAllstr;
    auto insertOneSpace = false;
    for( auto i = 0; i < fileBuffer.size(); ++i ) {
        char ch = fileBuffer[i];
        auto code = static_cast<int>(0xFFU & ch);
        auto it = m_charMapString.find(code);
        if ( it != m_charMapString.end() ) {
            // insertText(const QString & text)
            if ( insertPosType == 1 || insertPosType == 2  || insertPosType == 3 ) {
                // 1: empty buffer 
                // 2: insert in the middle
                // 3: insert after the end of space
                insertAllstr += QString("%1 ").arg( it.value() );
                // cursor.insertText( QString("%1 ").arg( it.value() ) );
            } else {
                // == 4
                if ( !insertOneSpace ) {
                    insertAllstr += QString(" ");
                    // cursor.insertText( tr(" ") );
                    insertOneSpace = true;
                }

                insertAllstr += QString("%1 ").arg( it.value() );
                // cursor.insertText( QString("%1 ").arg( it.value() ) );
            }
        }
        qDebug() << (i+1) << QString(". inserted  <%1>").arg( it.value() );
    }

    auto cursor = ui->textEdit->textCursor();
    cursor.insertText(insertAllstr);
    reportOperationResult( QString("Insert %1 byte(s) Successful ! ").arg(fileSz) , false);
}

//
// Saving
//
void MainWindow::on_saveBtn_clicked()
{
    QString errorMsg;
    auto bret = travelsalBinaryEditor(errorMsg);
    if ( !bret ) {
        reportOperationResult( errorMsg , true);
        return;
    }

    qint64 sz = static_cast<qint64>( m_BufferArray.size());
    if ( sz == 0 ) {
        reportOperationResult( "Buffer is Empty , do nothing" , false);
        return;
    }

    // m_BufferArray  prepare OK
    // QString	getSaveFileName(QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0)
    auto savedname = QFileDialog::getSaveFileName( this, tr("Save to File ...") );
    QFile savefile(savedname);
    if ( !savefile.open(QIODevice::WriteOnly) ) {
        reportOperationResult( QString("Can't open file : '%1'").arg(savedname) , true);
        return;
    } 

    // Open file OK , Do Saving
    auto savesz = savefile.write(m_BufferArray);
    savefile.flush();
    savefile.close();
    if ( savesz == sz ) {
        reportOperationResult( QString("Saved : '%1'").arg(savedname) , false);
    } else {
        reportOperationResult( QString("Something wrong when saving , saved buffer %1 != %2  @ file '%3'").arg(savesz).arg(sz).arg(savedname) , true);
    }
}

void MainWindow::on_moveBtn_clicked()
{
    auto strnumber = ui->moveCursorBox->text();
    if ( strnumber.isEmpty() ) {
        return;
    }

    auto convertOK = false;
    int num = strnumber.toInt(&convertOK, 10);
    if ( !convertOK ) {
        reportOperationResult("Please input a valid number to make movement", true);
        return;
    } else {
        if ( num < 0 ) {
            reportOperationResult("Negative movement is not allowed",true);
            return;
        }

        QString errorMsg;
        auto bret = travelsalBinaryEditor(errorMsg);
        if ( bret ) {
            int cnt = m_BufferArray.size();
            if ( num > cnt ) {
                reportOperationResult(QString("Movement number is out of range [0 ~ %1]").arg(cnt), true);
                return;
            }
        } else {
            reportOperationResult(QString("Parsed text editor Failed : %1").arg(errorMsg),true);
            return;
        }
    }

    QTextCursor cur = ui->textEdit->textCursor();
    if ( !cur.isNull() ) {
        cur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
        cur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, (num * modGrp) );
        ui->textEdit->setTextCursor(cur);

        reportOperationResult("Move Cursor Success", false);
        ui->textEdit->setFocus(Qt::OtherFocusReason);
    } else {
        reportOperationResult("Can't get Current Cursor", true);
    }
}

void MainWindow::on_selectBtn_clicked()
{
    auto strStartIdx = ui->lineEdit_2->text();
    auto strEndIdx = ui->lineEdit_3->text();   

    auto state = ui->checkBox->checkState();
    auto isLengthFlag = (state == Qt::Checked);

    if ( strStartIdx.isEmpty() || strEndIdx.isEmpty() ) {
        reportOperationResult("Start-Index or End-Index/Length is Empty", true);
        return;
    }

    auto b1 = false;
    auto b2 = false;
    int i_startIdx = strStartIdx.toInt(&b1,10);
    int i_endIdx = strEndIdx.toInt(&b2,10);
    if ( !b1 || !b2 ) {
        reportOperationResult("Start-Index or End-Index/Length can't convert to int", true);
        return;
    }

    QString errorMsg;
    auto bret = travelsalBinaryEditor(errorMsg);
    if ( !bret ) {
        reportOperationResult(QString("Parsed text editor Failed : %1").arg(errorMsg),true);
        return;
    }

    auto curSz = m_BufferArray.size();
    if( !(i_startIdx>=0 && i_startIdx <=curSz) ) {
        reportOperationResult(QString("Start-Index '%1' is invalid (out of range) : [0 ~ %2] ").arg(i_startIdx).arg(curSz) ,true);
        return;
    }
    // Start Index is OK

    QTextCursor cur = ui->textEdit->textCursor();
    if ( !isLengthFlag ) {
        // endIdx
        if( !(i_endIdx>=0 && i_endIdx <=curSz) ) {
            reportOperationResult(QString("End-Index '%1' is invalid (out of range) : [0 ~ %2] ").arg(i_endIdx).arg(curSz) ,true);
            return;
        }

        if ( i_endIdx < i_startIdx ) {
            reportOperationResult(QString("End-Index '%1' is invalid , must > %2 (Start-Index)").arg(i_endIdx).arg(i_startIdx) ,true);
            return;
        } 
        // endIndex OK
        cur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
        cur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, (i_startIdx * modGrp) );
        cur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, (i_endIdx-i_startIdx) * modGrp);
        ui->textEdit->setTextCursor(cur);
        ui->textEdit->setFocus(Qt::OtherFocusReason);
        reportOperationResult("Select Range Success", false);
    } else {
        // Length
        auto selLen = i_endIdx;
        if ( selLen == 0 ) {
            reportOperationResult( tr("Length is Zero , do nothing") ,false);
            return;
        } else if ( selLen > 0 ) {
            auto endNBytes = i_startIdx + selLen;
            auto warningFlag = (endNBytes > curSz);
            auto realEnd = warningFlag ? curSz : endNBytes;

            if ( warningFlag ) {
                reportOperationResult( QString("Positive Length is out of range , then select from '%1' to end").arg(i_startIdx), false);
            } else {
                reportOperationResult("Select Range Success", false);
            } 

            // Move to start
            cur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
            cur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, (i_startIdx * modGrp) );

            cur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, (realEnd-i_startIdx) * modGrp);
            ui->textEdit->setTextCursor(cur);
            ui->textEdit->setFocus(Qt::OtherFocusReason);
            // reportOperationResult("Select Range Success", false);
        } else {
            // < 0
            auto endNBytes = i_startIdx + selLen;
            auto warningFlag = (endNBytes < 0);
            auto realEnd = warningFlag ? 0 : endNBytes;

            if ( warningFlag ) {
                reportOperationResult( QString("Negative Length is out of range , then select from '%1' to head").arg(i_startIdx), false);
            } else {
                reportOperationResult("Select Range Success", false);
            } 

            // Move to start
            cur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
            cur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, (i_startIdx * modGrp) );

            cur.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, abs(i_startIdx - realEnd) * modGrp);
            ui->textEdit->setTextCursor(cur);
            ui->textEdit->setFocus(Qt::OtherFocusReason);
        }
    }



}


//
// Insert one Byte or a group of Bytes seperated by ','
//
void MainWindow::on_insertBtn_clicked()
{
    QString errorStr("[ERROR] : please input content from [00] ~ [FF]");
    auto inputBytes = ui->plainTextEdit->toPlainText().trimmed();
    if ( inputBytes.isEmpty() ) {
        reportOperationResult(errorStr, true);
        return;
    }

    auto cursor = ui->textEdit->textCursor();
    auto curPos = cursor.position();
    auto insertPosType = checkInsertPositionIsValid();
    if ( insertPosType == 0 ) {
        reportOperationResult( QString("Insert Position '%1' is not allowed! Valid position is begin / end / cursor index % 3 == 0").arg(curPos), true );
        return;
    }


    QString sepLetter(",");
    QString finalInsertText;
    auto foundCommaIdx = inputBytes.indexOf(sepLetter);
    if ( foundCommaIdx == -1 ) {
        // not found ","    Single Byte
        QString out;
        auto b = checkHexFormatValid(inputBytes, out);
        if ( !b ) {
            reportOperationResult( out, true );
            return;
        }

        // OK
        if ( insertPosType == 4 ) {
            // add a space @ front
            finalInsertText = QString(" %1").arg(out);
        } else {
            // add a space @ end
            finalInsertText = QString("%1 ").arg(out);
        }
        cursor.insertText(finalInsertText);
        reportOperationResult( tr("Insert 1 byte , Successful"), false );
    } else {
        auto bytesList = inputBytes.split(sepLetter);
        auto sepSz = bytesList.size();
        auto lastIdx = sepSz - 1;
        if ( sepSz > 0 ) {
            auto realInsertCnt = 0;
            auto meetError = false;
            QString errorMsg;
            for ( int i = 0; i < sepSz; ++i ) {
                QString ele = bytesList.at(i).trimmed();
                QString out;
                auto b = checkHexFormatValid(ele, out);

                if ( i != lastIdx ) {
                    if ( b ) {
                        finalInsertText += QString("%1 ").arg(out);
                        ++realInsertCnt;
                    } else {
                        errorMsg = out;
                        meetError = true;
                        break;
                    }
                } else {
                    // last one
                    if ( b ) {
                        finalInsertText += QString("%1 ").arg(out);
                        ++realInsertCnt;
                    } else {
                        if ( !ele.isEmpty() ) {
                            errorMsg = out;
                            meetError = true;
                            break;
                        } else {
                            // last element is ""  is an empty string : OK
                        }
                    }
                }
            }

            if ( meetError ) {
                reportOperationResult(errorMsg, true );
                return;
            }

            // succ : insert final text
            if ( insertPosType == 4 ) {
                // prepend a space @ inserted position
                finalInsertText = QString(" %1").arg(finalInsertText);
            }
            cursor.insertText(finalInsertText);
            reportOperationResult( QString("Insert %1 byte(s) , Successful").arg(realInsertCnt), false );
        } else {
            // Seperate an empty string list , do nothing
        }
    }
}

void MainWindow::on_summaryBtn_clicked()
{
    onBinEditorTextContentChanged();
}


void MainWindow::on_checkBox_stateChanged(int checkState)
{
    if ( checkState == Qt::Unchecked ) {
        ui->label_3->setText( tr("End Position") );
    } else {
        // Checked
        ui->label_3->setText( tr("Length") );
    }
}


void MainWindow::onBinEditorTextCursorChanged()
{
    QTextCursor cur = ui->textEdit->textCursor();
    if ( !cur.isNull() ) {
        auto pos =  cur.position();
        auto bret = true;
        QString errorMsg;
        if ( m_BufferChangedFlag ) {
            bret = travelsalBinaryEditor(errorMsg);
            m_BufferChangedFlag = false;
        }

        if ( bret ) {
            auto idx = pos / modGrp;
            ui->statusBar->showMessage( QString("Cursor Position Changed : Current = %1 , Byte Index = %2").arg(pos).arg(idx) );
        } else {
            ui->statusBar->showMessage( QString("Cursor Position Changed : Current = %1").arg(pos) );
        }
    }
}

void MainWindow::onBinEditorTextContentChanged()
{
    QString errorMsg;
    auto bret = travelsalBinaryEditor(errorMsg);
    if ( bret ) {
        auto outputstr = getPrintOutBuffer();
        ui->plainTextEdit_2->setPlainText(outputstr);
    } else {
        ui->plainTextEdit_2->setPlainText( QString("Some wrong with the Buffer! \n%1").arg(errorMsg) );
        reportOperationResult(errorMsg,true);
    }

    m_BufferChangedFlag = true;
}


bool MainWindow::travelsalBinaryEditor_Unused(QString& errMsg)
{
    errMsg = "";
    m_BufferArray.clear();

    auto pDocument = ui->textEdit->document();
    if ( pDocument == nullptr ) {
        return false;
    }

    // qDebug() << "blockCount = " << pDocument->blockCount();
    auto meetError = false;
    // int cnt = 0;
    QString hexprefix("Hex-");
    // qDebug() << "travelsal All Blocks : ";
    for ( auto it = pDocument->begin(); it != pDocument->end(); it = it.next()  /* , ++cnt */) {
        // qDebug() << "\tvisit Block , next begin-for" << cnt;
        // fragment()
        int idxForFrag = 0;

        // QTextBlock::iterator it;
        for ( auto blkIt = it.begin(); !(blkIt.atEnd());  ++blkIt, ++idxForFrag ) {
            auto frag = blkIt.fragment();
            // qDebug() << "\t\tvisit fragment " << idxForFrag << " , len = " << frag.length();
            // frag.length();
            QTextImageFormat newImageFormat = frag.charFormat().toImageFormat();

            if ( newImageFormat.isValid() ) {
                auto imgName = newImageFormat.name();
                if ( imgName.startsWith(hexprefix) ) {
                    auto hexName = imgName.mid(hexprefix.size() );
                    QByteArray ba = hexName.toLatin1();
                    if ( !checkCharByteArrayValid(ba) ) {
                        meetError = true;
                        errMsg  = QString("Found imgName '%1' is not in the range : [00] ~ [FF]").arg(hexName);
                    } else {
                        auto bFlag = false;
                        int code = ba.toInt(&bFlag,16);
                        if ( !bFlag ) {
                            meetError = true;
                            errMsg = QString("cast 0x'%1' is not valid [00] ~ [FF]").arg(hexName);
                        } else {
                            char ch = static_cast<char>(0xFFU & code);
                            for ( int i = 0; i < frag.length(); ++i ) { 
                                m_BufferArray.append(ch);
                            }
                        }
                    }
                } else {
                    meetError = true;
                    errMsg = QString("Found imgName '%1' is not starts with %2").arg(imgName).arg(hexprefix);
                }
                // qDebug() << "\t\tfragment " << idxForFrag << " is a image format , name => " << newImageFormat.name();
                // frag.length() *   Hex-%1
            } else {
                QTextCharFormat chfmt = frag.charFormat().toCharFormat();
                if ( chfmt.isValid() ) {
                    meetError = true;
                    errMsg = QString("Input text '%1' is not allowed").arg(frag.text());
                    // qDebug() << "\t\tfragment " << idxForFrag << " is a <Char-Char-Char> format => " << frag.text();
                } else {
                    meetError = true;
                    errMsg = QString("Known format of Input content");
                    // qDebug() << "\t\tfragment " << idxForFrag << " is an other format";
                }
            }

            if ( meetError ) {
                break;
            }
        }

        if ( meetError ) {
            break;
        }

        // QTextCharFormat chfmt = it.charFormat();
        // int tp = chfmt.objectType();
        // switch ( tp ) 
        // {
        // case QTextFormat::NoObject:
        //     qDebug() << cnt << ". NoObject"; 
        //     break;
        // case QTextFormat::ImageObject:	
        //     qDebug() << cnt << ". ImageObject"; 
        //     break;
        // case QTextFormat::TableObject:
        //     qDebug() << cnt << ". TableObject"; 
        //     break;
        // case QTextFormat::TableCellObject:
        //     qDebug() << cnt << ". TableCellObject"; 
        //     break;
        // case QTextFormat::UserObject:
        //     qDebug() << cnt << ". UserObject"; 
        //     break;
        // default:
        //     qDebug() << cnt << ". in switch-case default , It's so Strange!!!"; 
        //     break;
        // }


    }
    // qDebug() << "cnt = " << cnt;




    return !meetError;

}

bool MainWindow::travelsalBinaryEditor(QString& errMsg)
{
    using namespace charRange;

    errMsg = "";
    m_BufferArray.clear();
    QString content = ui->textEdit->toPlainText();

    if ( content.isEmpty() ) {
        return true;
    }

    auto needForceCapitalLetterFlag = true;
    auto contentSz = content.size();
    auto meetError = false;
    QByteArray singleByteBuf;
    for ( auto i = 0; i < contentSz; ++i ) {
        QChar ch = content.at(i);
        auto modIdx = i % modGrp;
        if ( modIdx == 0 ) {
            singleByteBuf.clear();
            if ( !isQCharInRange(ch,needForceCapitalLetterFlag) ) {
                meetError = true;
                errMsg = QString("char [%1] => '%2' is Invalid. Valid range is [0-9] or [A-F] ").arg(i).arg(ch);
                break;
            } else {
                singleByteBuf.append(ch);
            }
        } else if ( modIdx == 1 ) {
            if ( !isQCharInRange(ch,needForceCapitalLetterFlag) ) {
                meetError = true;
                errMsg = QString("char [%1] => '%2' is Invalid. Valid range is [0-9] or [A-F] ").arg(i).arg(ch);
                break;
            } else {
                singleByteBuf.append(ch);

                auto convertFlag = false;
                int code = singleByteBuf.toInt(&convertFlag,16);
                if ( !convertFlag ) {
                    meetError = true;
                    errMsg = QString("singleByteBuf '%1'   can't convert to int on base 16").arg( QString(singleByteBuf) );
                    break;
                } else {
                    char ch = static_cast<char>(code);
                    m_BufferArray.append(ch);
                }
            }
        } else {
            // modIdx == 2
            if ( ch != sc_space ) {
                meetError = true;
                errMsg = QString("char [%1] => '%2' is not  <Space>. ").arg(i).arg(ch);
                break;
            }
        }
    }

    // clear real buffer if neccessary
    if ( meetError ) {
        m_BufferArray.clear();
    }

    return !meetError;
}


void MainWindow::on_cleanErrMsgBtn_clicked()
{
    ui->errorBox->setPlainText( tr("") );
}



void MainWindow::reportOperationResult(const QString& msg, bool isError)
{
    if ( !msg.isEmpty() ) {
        ui->errorBox->setPlainText( tr("") );

        if ( isError ) {
            auto colorMsg = QString("<font color='#FF0000'>%1</font>").arg(msg);
            ui->errorBox->appendHtml( colorMsg );
            ui->statusBar->showMessage( msg );
        } else {
            ui->errorBox->setPlainText( msg );
            ui->statusBar->showMessage( msg );
        }
    }

}


bool MainWindow::checkCharByteArrayValid(const QByteArray& ba)
{
    if ( ba.size() != 2 ) {
        return false;
    }

    auto isOK = true;
    for( int i = 0; i < 2; ++i ) {
        auto ch = ba[i];
        auto isValid = ((ch >='0' && ch <='9')  ||  (ch>='A' && ch <='F' ));
        if ( !isValid ) {
            isOK = false;
            break;
        }
    }

    return isOK;
}


QString MainWindow::getPrintOutBuffer()
{
    QString retstr;

    auto pos = 0;
    QTextCursor cur = ui->textEdit->textCursor();
    if ( !cur.isNull() ) {
       pos =  cur.position();
    }
    auto bufsz = m_BufferArray.size();
    auto decimalBits = 0;
    auto tmpSz = bufsz;
    // calc bit
    do {
        tmpSz /= 10;
        ++decimalBits;
    } while( tmpSz!=0 );

    retstr += QString("--------------------\n");
    retstr += QString("current cursor position : %1\n").arg(pos);
    retstr += QString("current buffer size : %1\n").arg( bufsz );
    retstr += QString("--------------------\n");
    if ( bufsz == 0 ) {
        retstr += QString("<Empty Buffer>\n");
    } else {
        for ( int i = 0; i < bufsz; ++i ) {
            char ch = m_BufferArray[i];
            auto code = static_cast<int>(0xFFU & ch);
            // qDebug() << i+1 << ". " << code;
            auto it = m_charMapString.find(code);
            if ( it != m_charMapString.end() ) {
                QString strIdx;
                strIdx.setNum(i+1, 10);
                strIdx = strIdx.rightJustified(decimalBits, ' ');

                retstr += QString("%1.   %2 \n").arg(strIdx).arg( it.value() );
            } else {
                // qDebug() << i+1 << ". Not Found";
            }
        }
    }
    
    return retstr;
}



bool MainWindow::isQCharInRange(const QChar& ch, bool needForceCapitalLetter)
{
    using namespace charRange;
    if ( needForceCapitalLetter ) {
        return    (ch>=sc_num0 && ch<=sc_num9) 
                || (ch>=sc_numA && ch<=sc_numF);
    } else {
        // a-f / A-F are both  valid
        return    (ch>=sc_num0 && ch<=sc_num9) 
                || (ch>=sc_numA && ch<=sc_numF)
                || (ch>=sc_numa && ch<=sc_numf);
    }

}



int  MainWindow::checkInsertPositionIsValid()
{
    auto curPos = ui->textEdit->textCursor().position();
    auto byteSz = m_BufferArray.size();
    auto insertPosType = 0;
    if ( byteSz == 0 ) {
        insertPosType = 1;  // insert at begin and buffer is empty
    } else {
        auto lastIdx1 = byteSz * modGrp;
        auto lastIdx2 = lastIdx1 - 1;

        if ( curPos == lastIdx1  ) {
            insertPosType = 3;  // insert at end with Last Space
        } else if ( curPos == lastIdx2 ) {
            insertPosType = 4;  // insert at end <Without> Last Space
        } else {
            if ( curPos % modGrp == 0 ) {
                insertPosType = 2;  // insert at Middle , Before A certain byte
            }
        }
    }

    return insertPosType;
}



bool MainWindow::checkHexFormatValid(const QString& in, QString& out)
{
    out = "";
    auto strSz = in.size();
    if ( !(strSz>=1 && strSz<=4) ) {
        out = QString("input text.size =  '%1' => is invalid. Valid insert string must be size [1~4]").arg(strSz);
        return false;        
    }

    QString invalidMsgFmt("char [%1]  Invalid. Valid range is [0-9] or [A-F] !");
    auto checkRet = false;
    auto needForceCapitalLetterFlag = false;
    if ( strSz == 1 ) {
        QChar oneLetter(in.at(0));
        checkRet = isQCharInRange(oneLetter, needForceCapitalLetterFlag);
        if ( !checkRet ) {
            out = invalidMsgFmt.arg(oneLetter);
        } else {
            QString fmtstr = in.rightJustified(2,'0');
            fmtstr = fmtstr.toUpper();
            out = fmtstr;
        }
    } else if ( strSz == 2 ) {
        // in = in.toUpper();
        auto errorIdx = -1;
        auto meetError = false;
        for ( int i = 0; i < strSz; ++i ) {
            if ( !isQCharInRange(in.at(i), needForceCapitalLetterFlag) ) {
                errorIdx = i;
                meetError = true;
                break;
            }
        }

        if ( meetError ) {
            out = invalidMsgFmt.arg( in.at(errorIdx) );
            checkRet = false;
        } else {
            checkRet = true;
            out = in.toUpper();
        }
    } else {
        // sz == 3 or 4    ,  must start with  0x  0X
        if ( !(in.startsWith("0x") || in.startsWith("0X")) ) {
            checkRet = false;
            out = "input text.size == 3 or 4, but start string is not  0x or 0X";
        } else {
            QString reststr = in.mid(2);

            auto errorIdx = -1;
            auto meetError = false;
            for ( int i = 0; i < reststr.size(); ++i ) {
                if ( !isQCharInRange(reststr.at(i), needForceCapitalLetterFlag) ) {
                    errorIdx = i;
                    meetError = true;
                    break;
                }
            }

            if ( meetError ) {
                out = invalidMsgFmt.arg( reststr.at(errorIdx) );
                checkRet = false;
            } else {
                checkRet = true;
                QString fmtstr = reststr.rightJustified(2,'0');
                fmtstr = fmtstr.toUpper();
                out = fmtstr;
            }
        }
        
    }

    
    return checkRet;
}


void MainWindow::onInvalidKeyInput(const QString& msg, bool errorFlag)
{
    if ( errorFlag ) {
        reportOperationResult(msg, errorFlag);
    } else {
        ui->errorBox->setPlainText( msg );
    }
}
