#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fnedcore.h"

#include <QElapsedTimer>
#include <QFileInfo>
#include <QDebug>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_encBtn_clicked()
{
    QElapsedTimer timer_e;
    timer_e.start();

    // Process File Path
    QString filePath = ui->filePathWnd->toPlainText().trimmed();
    if ( filePath.endsWith( QStringLiteral(".")  ) ) {
        ui->logWnd->clear();
        ui->logWnd->setPlainText( QString("File can't be ends with '.'  ") );
        return;
    }

    // qDebug() << "filePath = " << filePath;
    QFileInfo fInfo(filePath);
    if ( !fInfo.isFile() && !fInfo.exists() ) {
        ui->logWnd->clear();
        ui->logWnd->setPlainText( QString(" It's not a vaild file Path! ") );
        return;
    }

    // encode1File_TestCase(filePath);
    encode_a_GivenFile(filePath);
}



void MainWindow::on_decBtn_clicked()
{
    QElapsedTimer timer_e;
    timer_e.start();

    QString filePath = ui->filePathWnd->toPlainText().trimmed();
    if ( filePath.endsWith( QStringLiteral(".")  ) ) {
        ui->logWnd->clear();
        ui->logWnd->setPlainText( QString("File can't be ends with '.'  ") );
        return;
    }

    // qDebug() << "filePath = " << filePath;
    QFileInfo fInfo(filePath);
    if ( !fInfo.isFile() && !fInfo.exists() ) {
        ui->logWnd->clear();
        ui->logWnd->setPlainText( QString(" It's not a vaild file Path! ") );
        return;
    }

    ui->logWnd->clear();

    // decode1File_TestCase(filePath);
    decode_a_GivenFile(filePath);
}




void MainWindow::on_encWithArgsBtn_clicked()
{
    // Process File Path
    QString filePath = ui->filePathWnd->toPlainText().trimmed();
    if ( filePath.endsWith( QStringLiteral(".")  ) ) {
        ui->logWnd->clear();
        ui->logWnd->setPlainText( QString("File can't be ends with '.'  ") );
        return;
    }

    // qDebug() << "filePath = " << filePath;
    QFileInfo fInfo(filePath);
    if ( !fInfo.isFile() && !fInfo.exists() ) {
        ui->logWnd->clear();
        ui->logWnd->setPlainText( QString(" It's not a vaild file Path! ") );
        return;
    }


    QElapsedTimer timer_e;
    timer_e.start();

    EncArgWrap obj;
    obj.additionalExp = ui->spinBox->value();    // [4,6]
    obj.specialCharIdx = ui->spinBox_2->value(); // [0,5)
    obj.nShiftBits = ui->spinBox_3->value();     // [1,8)
    obj.bShiftDir = ui->spinBox_4->value();      //  0(<<) or 1(>>)
    obj.bBigOrLittle = ui->spinBox_5->value();   //  0:Big Endian    1:Little Endian

    QString error;
    FNEDCore enc;
    if ( enc.encryptFile_withArg(filePath, FNEDCore::genIDByArgs(obj), error) ) {
        qDebug() << "[DONE]  All Finished successfully ! ";
    } else {
        ui->logWnd->clear();
        ui->logWnd->setPlainText(error);
    }

    qDebug() << "Encode with Args Used : " << timer_e.elapsed() / 1000.0 << " s";
}

void MainWindow::on_cmp2FilesBtn_clicked()
{
    QElapsedTimer timer_e;
    timer_e.start();

    QString sepFilePath = ui->filePathWnd->toPlainText().trimmed();
    auto sepList = sepFilePath.split( QString("\n") );
    if ( sepList.size() != 2 ) {
        ui->logWnd->clear();
        ui->logWnd->setPlainText( QString("Can't seperate string by \\n") );
        return;
    }


    qDebug() << "file1 : " << sepList.at(0);
    qDebug() << "file2 : " << sepList.at(1);

    QString error;
    ui->logWnd->clear();
    if ( FNEDCore::is2FileEqual( sepList.at(0), sepList.at(1),  error) ) {
        ui->logWnd->setPlainText( QString("[DONE] 2 File are equal at all! elapsed time : %1 ").arg(  timer_e.elapsed() / 1000.0  ) );
    } else {
        ui->logWnd->setPlainText( error + QString(", elapsed time : %1 ").arg( timer_e.elapsed() / 1000.0 ) );
    }
}


void MainWindow::encode1File_TestCase(const QString& filePath)
{
    QElapsedTimer timer_e;
    timer_e.start();

    FNEDCore enc;
    QString error;
    ui->logWnd->clear();
    if ( enc.encryptFile_TestAll(filePath, error ) ) {
        ui->logWnd->setPlainText(QString("[DONE]  All Finished successfully ! elapsed time : %1 s. ").arg( timer_e.elapsed() / 1000.0 ) );
    } else {
        ui->logWnd->setPlainText(QString("%1 | elapsed time : %2 s. ").arg(error).arg( timer_e.elapsed() / 1000.0 ) );
    }
}

void MainWindow::decode1File_TestCase(const QString& filePath)
{
    QElapsedTimer timer_e;
    timer_e.start();

    FNEDCore dec;
    QString error;
    if ( dec.decryptFile_TestAll_WithCmp(filePath,error) ) {
        ui->logWnd->setPlainText( QString("[DONE] Decode File successfully ! elapsed time : %1 s.").arg( timer_e.elapsed() / 1000.0 ) );
    } else {
        ui->logWnd->setPlainText( QString("%1 | elapsed time %2 s ").arg(error).arg( timer_e.elapsed() / 1000.0 ) );
    }
}

void MainWindow::encode_a_GivenFile(const QString& filePath)
{
    QElapsedTimer timer_e;
    timer_e.start();

    FNEDCore enc;
    QString error;
    ui->logWnd->clear();
    if ( enc.encryptFile( filePath, error ) ) {
        ui->logWnd->setPlainText(QString("[DONE] encode the given file %1 Successfully  ! elapsed time : %2 s. ").arg(filePath).arg( timer_e.elapsed() / 1000.0  ) );
    } else {
        ui->logWnd->setPlainText(QString("%1 | encode elapsed time : %2 s. ").arg(error).arg( timer_e.elapsed() / 1000.0 ) );
    }
}

void MainWindow::decode_a_GivenFile(const QString& filePath)
{
    QElapsedTimer timer_e;
    timer_e.start();

    FNEDCore dec;
    QString restoreFileName;
    QString error;
    ui->logWnd->clear();
    if ( dec.decryptFile( filePath, restoreFileName,  error ) ) {
        ui->logWnd->setPlainText(QString("[DONE] decode the given file %1 -> %2 Successfully  ! elapsed time : %3 s. ").arg(filePath).arg( restoreFileName ).arg( timer_e.elapsed() / 1000.0 ) );
    } else {
        ui->logWnd->setPlainText(QString("%1 | decode elapsed time : %2 s. ").arg(error).arg( timer_e.elapsed() / 1000.0  ) );
    }
}

