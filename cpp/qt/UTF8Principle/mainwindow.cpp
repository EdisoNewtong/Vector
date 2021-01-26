#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QVector>
#include <QByteArray>
#include <QFontComboBox>
#include <QFont>


namespace ConstInvalid { 
    static const uint invalid_Unicode32 = 0xFFFDU;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_stringMode(true)
    , m_unicodeMode(true)
    , m_timerStart(false)
    , m_curFontIdx(0)
{
    ui->setupUi(this);
    m_fontTimer.stop();
    connect(&m_fontTimer, SIGNAL(timeout()), this, SLOT(updateFont()) );
    connect(ui->fontComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectFontChanged(int)) );

    initUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
    ui->qstringRadBtn->setChecked(true);
    ui->unicodeRadBtn->setChecked(false);

    ui->uniBtn->setChecked(true);
    ui->utf8Btn->setChecked(false);

    updateGrp();
    updateSub();

    // auto fnt = ui->plainTextEdit->font();
    ui->fontComboBox->setCurrentFont( ui->plainTextEdit->font() );
    ui->lineEdit_6->setText( tr("1500") );
}

void MainWindow::on_qstringRadBtn_clicked()
{
    // qDebug() << "QString Mode Select";
    m_stringMode = true;
    updateGrp();
}

void MainWindow::on_unicodeRadBtn_clicked()
{
    // qDebug() << "Unicode/UTF-8 Mode Select";
    m_stringMode = false;
    updateGrp();
}

void MainWindow::on_uniBtn_clicked()
{
    // qDebug() << "Unicode32 Sub-Mode Select";
    m_unicodeMode = true;
    updateSub();
}

void MainWindow::on_utf8Btn_clicked()
{
    // qDebug() << "UTF-8 Sub-Mode Select";
    m_unicodeMode = false;
    updateSub();
}

// Calc
void MainWindow::on_pushButton_clicked()
{
    // ui->plainTextEdit_2
    // unicode32
    // utf8_1 utf8_2 utf8_3 utf8_4

    // ui->plainTextEdit
    
    // toUcs4()
    QString retstr;
    if ( m_stringMode ) {
        retstr = calcQString( ui->plainTextEdit_2->toPlainText() );
        ui->plainTextEdit->setPlainText(retstr);
    } else {
        if ( m_unicodeMode ) {
            retstr = calcU32( ui->unicode32->text() );
        } else {
            retstr = calcUtf8(ui->utf8_1->text(), ui->utf8_2->text(), ui->utf8_3->text(), ui->utf8_4->text() );
        }

        ui->plainTextEdit->setPlainText(retstr);
    }
}

// Timer Switch Font
void MainWindow::on_pushButton_2_clicked()
{

    m_timerStart = !m_timerStart;
    if ( !m_timerStart ) {
        m_fontTimer.stop();

        auto savedIdx = m_curFontIdx;
        auto cnt = ui->fontComboBox->count();
        auto curFont = ui->fontComboBox->currentFont();
        auto fontName = curFont.family();
        ui->pushButton_2->setText( tr("Switch Font") );

        ui->statusBar->showMessage( QString("Timer Stop!! Current Font : ( %1 / %2 ) => %3").arg(savedIdx).arg(cnt).arg(fontName) );
        return;
    }

    ui->pushButton_2->setText( tr("Stop Switch") );
    m_curFontIdx = 0;
    // Start it
    auto strms = ui->lineEdit_6->text();
    auto bConvertFlag = false;
    int ms = strms.toInt(&bConvertFlag,10);
    if ( !bConvertFlag ) {
        ui->statusBar->showMessage( QString("Can't convert '%1' into an int").arg(strms) );
        return;
    }

    if ( ms <=0 ) {
        ui->statusBar->showMessage( QString("[Error] : '%1' <=0 ").arg(ms) );
        return;
    }
    m_fontTimer.start(ms);
}


void MainWindow::updateGrp()
{
    if ( m_stringMode ) {
        ui->qstringGrp->setEnabled(true);
        ui->unicodeGrp->setEnabled(false);
    } else {
        ui->qstringGrp->setEnabled(false);
        ui->unicodeGrp->setEnabled(true);
    }
}

void MainWindow::updateSub()
{
    if ( m_unicodeMode ) {
        // Unicode-32  : U+ xxxx
        ui->unicode32->setEnabled(true);
        ui->utf8_1->setEnabled(false);
        ui->utf8_2->setEnabled(false);
        ui->utf8_3->setEnabled(false);
        ui->utf8_4->setEnabled(false);
    } else {
        // UTF-8  : 
        ui->unicode32->setEnabled(false);
        ui->utf8_1->setEnabled(true);
        ui->utf8_2->setEnabled(true);
        ui->utf8_3->setEnabled(true);
        ui->utf8_4->setEnabled(true);
    }
}



QString MainWindow::calcQString(const QString& content)
{
    using namespace ConstInvalid;
    QString retstr;
    if ( content.isEmpty() ) {
        ui->statusBar->showMessage( tr("Empty Content") );
        return retstr;
    }

    retstr += "----------\n";
    retstr += QString("%1\n").arg(content);
    retstr += "----------\n";

    int characterCnt = 0;
    int idx = 0;
    int qstringsz = content.size();
    QString inner;
    while ( idx < qstringsz ) {
        QString singleCh = content.mid(idx,1);
        auto vec32 = singleCh.toUcs4();
        if ( vec32.size() > 0 && vec32.at(0) == invalid_Unicode32 ) {
            singleCh = content.mid(idx,2);
            auto vec32_2 = singleCh.toUcs4();
            if ( vec32_2.size() > 0 && vec32_2.at(0) != invalid_Unicode32 ) {
                uint u32code = vec32_2.at(0);
                auto sret = auxOutput(singleCh,u32code);
                inner += QString("%1. '%2' => 2 QChars , %3\n").arg(characterCnt+1).arg(singleCh).arg(sret);
            } else {
                inner += QString("%1. '%2' => 2 QChars , %3\n").arg(characterCnt+1).arg(singleCh).arg("??? Unknow U32 / UTF-8");
            }

            ++characterCnt;
            idx += 2;
        } else {
            // Valid
            uint u32code = vec32.at(0);
            auto sret = auxOutput(singleCh,u32code);
            inner += QString("%1. '%2' => 1 QChar , %3 \n").arg(characterCnt+1).arg(singleCh).arg(sret);

            ++characterCnt;
            idx += 1;
        }
    }

    retstr += QString("\t size() = %1 QChar(s), character(s) = %2 \n").arg(content.size()).arg(characterCnt);
    retstr += inner;

    return retstr;
}

QString MainWindow::calcU32(const QString& content)
{
    QString retstr;
    if ( content.isEmpty() ) {
        ui->statusBar->showMessage( tr("Empty Content") );
        retstr = "";
        return retstr;
    }
    
    auto bConvertFlag = false;
    uint code32 = content.toUInt(&bConvertFlag,16);
    if ( !bConvertFlag ) {
        retstr = QString("Can't convert '%1' into hex code").arg(content);
        return retstr;
    }

    int szOfAry = 1;
    // szOfAry =>  &code32  is the address of an array with only 1 element ( array's size = 1 )
    QString singleCh = QString::fromUcs4(&code32,szOfAry);
    // qDebug() << "singleCh = " << singleCh;
    QString detailstr = auxOutput(singleCh, code32);
    retstr = QString("'%1' => \n%2").arg(singleCh).arg(detailstr);
    return retstr;
}

QString MainWindow::calcUtf8(const QString& u1, const QString& u2, const QString& u3, const QString& u4)
{
    using namespace ConstInvalid;
    QString retstr;
    if ( u1.isEmpty() && u2.isEmpty() && u3.isEmpty() && u4.isEmpty() ) {
        ui->statusBar->showMessage( tr("Empty Content") );
        retstr = "";
        return retstr;
    }

    QString realu1 = u1;
    if ( realu1.isEmpty() ) {
        realu1 = "00";
    } 
    QString realu2 = u2;
    if ( realu2.isEmpty() ) {
        realu2 = "00";
    } 
    QString realu3 = u3;
    if ( realu3.isEmpty() ) {
        realu3 = "00";
    }
    QString realu4 = u4;
    if ( realu4.isEmpty() ) {
        realu4 = "00";
    }

    auto b1 = false;
    auto b2 = false;
    auto b3 = false;
    auto b4 = false;
    int code1 = realu1.toInt(&b1,16);
    if ( !b1 ) {
        retstr = QString("convert to int , byte-1 '%1' by hex code Failed").arg(realu1);
        return retstr;
    }

    int code2 = realu2.toInt(&b2,16);
    if ( !b2 ) {
        retstr = QString("convert to int , byte-2 '%1' by hex code Failed").arg(realu2);
        return retstr;
    }
    int code3 = realu3.toInt(&b3,16);
    if ( !b3 ) {
        retstr = QString("convert to int , byte-3 '%1' by hex code Failed").arg(realu3);
        return retstr;
    }
    int code4 = realu4.toInt(&b4,16);
    if ( !b4 ) {
        retstr = QString("convert to int , byte-4 '%1' by hex code Failed").arg(realu4);
        return retstr;
    }

    QByteArray ba;
    ba += static_cast<char>(code1);
    ba += static_cast<char>(code2);
    ba += static_cast<char>(code3);
    ba += static_cast<char>(code4);
    // qDebug() << "ba = " << ba;

    QString singleCh = QString::fromUtf8(ba);
    // qDebug() << "utf8 : " << singleCh;
    auto vec32 = singleCh.toUcs4();
    if ( vec32.size() > 0 && vec32.at(0) != invalid_Unicode32 ) {
        QString detailstr = auxOutput(singleCh, vec32.at(0) );
        retstr = QString("'%1' => \n%2").arg(singleCh).arg(detailstr);
    } else {
        retstr = QString("Can't convert from UTF-8 Code to a MultiByte Character QString");
    }

    return retstr;
}


QString MainWindow::auxOutput(const QString& singleCh, unsigned int u32code)
{
    QString retstr;
    QByteArray ba = singleCh.toUtf8();
    retstr += " utf-8 : \"";
    for ( int i = 0; i < ba.size(); ++i ) {
        char ch = ba[i];
        int code = static_cast<int>(0xFFU & ch);

        QByteArray chba;
        chba.setNum(code,16);
        chba = chba.rightJustified(2,'0');
        chba = chba.toUpper();
        retstr += QString("\\x%1").arg( QString(chba));
    }
    retstr += "\"";


    QByteArray hexAry;
    hexAry.setNum(u32code, 16);
    hexAry = hexAry.rightJustified(2,'0');
    hexAry = hexAry.toUpper();
    // qDebug() << "ba = " << QString(ba);
    retstr += QString(", u32 : U+%1").arg( QString(hexAry) );

    return retstr;

}


void MainWindow::updateFont()
{
    auto cnt = ui->fontComboBox->count();
    ++m_curFontIdx;
    m_curFontIdx %= cnt;
    ui->fontComboBox->setCurrentIndex(m_curFontIdx);
    
    auto curFont = ui->fontComboBox->currentFont();
    auto fontName = curFont.family();
    ui->plainTextEdit->setFont(curFont);
    ui->plainTextEdit_2->setFont(curFont);

    ui->statusBar->showMessage( QString("Current Font : ( %1 / %2 ) => %3").arg(m_curFontIdx).arg(cnt).arg(fontName) );
}


void MainWindow::onSelectFontChanged(int idx)
{
    auto cnt = ui->fontComboBox->count();
    auto curFont = ui->fontComboBox->currentFont();
    auto fontName = curFont.family();

    ui->plainTextEdit->setFont(curFont);
    ui->plainTextEdit_2->setFont(curFont);

    ui->statusBar->showMessage( QString("Picked Font : ( %1 / %2 ) => %3").arg(idx).arg(cnt).arg(fontName) );

}

