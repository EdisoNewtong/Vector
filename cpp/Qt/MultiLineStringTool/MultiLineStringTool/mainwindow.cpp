#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QVector>
#include <QRandomGenerator>

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


void MainWindow::on_convertBtn_clicked()
{
    static const QString SC_DOUBLE_QUOTE("\"");
    static const QString SC_REAL_NEW_LINE("\n");
    static const QString SC_STR_NEW_LINE("\\n");
    static const QString sc_rawEndDelimiter( ")\"" );

    QString oriTxt = ui->src->toPlainText();
    QString oriTxtCp(oriTxt); // backup a copy
    // auto bFoundCloseDelimiter = oriTxt.contains(sc_rawEndDelimiter);
    oriTxt.replace( QChar('\\'), QString("\\\\") );
    oriTxt.replace( QChar('\''), QString("\\\'") );
    oriTxt.replace( QChar('\"'), QString("\\\"") );

    auto lineList = oriTxt.split( QChar('\n') );
    auto bMtFlag = (ui->sugarChk->checkState() == Qt::Checked);

    ui->mtl->clear();
    QString cvtStr;
    if ( !bMtFlag ) {
        cvtStr += SC_DOUBLE_QUOTE;
    }

    // qDebug() << "lines = " << lineList.size();
    // qDebug() << "lastlineEmpty = " << (lineList.last().isEmpty() ? " true " : " false ");
    for ( int i = 0; i < lineList.size(); ++i ) {
        QString lineStr( lineList[i] );
        auto bisLastLine = ( i == ( lineList.size()-1 ) );
        if ( !bisLastLine ){
            if ( bMtFlag ) {
                // content
                cvtStr += SC_DOUBLE_QUOTE;
                  cvtStr += lineStr;
                  cvtStr += SC_STR_NEW_LINE;
                cvtStr += SC_DOUBLE_QUOTE;
                // \n
                cvtStr += SC_REAL_NEW_LINE;
            } else {
                cvtStr += lineStr;
                cvtStr += SC_STR_NEW_LINE;
            }
        } else {
            // is Last String
            if ( !lineStr.isEmpty() ) {
                if ( bMtFlag ) {
                    cvtStr += SC_DOUBLE_QUOTE;
                      cvtStr += lineStr;
                    cvtStr += SC_DOUBLE_QUOTE;
                } else {
                    cvtStr += lineStr;
                }
            } 
        }

    }

    if ( !bMtFlag ) {
        cvtStr += SC_DOUBLE_QUOTE;
    }
    ui->mtl->setPlainText( cvtStr );

    // set C++11 Raw String
    if ( !oriTxtCp.contains(sc_rawEndDelimiter)  ) {
        ui->raw->setPlainText( QString("R\"(%1)\"").arg( oriTxtCp  )  );
    } else {
        // found    )"  from      line#1 )"
        //                        line#2
        //

        QVector<char> pickedPool;
        // start after char <Space> (asciiCode = 32) ' ' 
        // char must be except the backsplash, whitespace and parentheses
        for( char ch = 33; ch < 127; ++ch ) { 
            if ( ch!='\\' && ch!='(' && ch!=')'  ) { 
                pickedPool.push_back( ch ); 
            } 
        }


        const int MAX_CH_CNT = 16;
        const int try_cnt = 10;
        QString closeStr;
        QString delimiter;
        bool bFoundCloseFlag = false;

        int gen_cnt = 3;
        do {
            for( int ccnt = 0; ccnt < try_cnt; ++ccnt ) {
                delimiter.clear();

                int pre_idx = -1;
                for( int i = 0; i < gen_cnt; ++i ) {
                    int gen_idx = 0;
                    do {
                        gen_idx = QRandomGenerator::global()->bounded( 0, pickedPool.size() );
                    } while( gen_idx == pre_idx );

                    pre_idx = gen_idx;
                    delimiter += pickedPool[ gen_idx ];
                }
                closeStr = QString(")%1\"").arg( delimiter );
                // qDebug() << "dim = " << delimiter;
                // qDebug() << "closeStr = " << closeStr;

                if ( !oriTxtCp.contains(closeStr) ) {
                    bFoundCloseFlag = true;
                    break;
                }
            }

            if ( bFoundCloseFlag ) {
                break;
            }
        } while ( ++gen_cnt <= MAX_CH_CNT );

        if ( bFoundCloseFlag ) {
            ui->raw->setPlainText( QString("R\"%1(%2)%3\"")
                                             .arg( delimiter )
                                             .arg( oriTxtCp )
                                             .arg( delimiter )  );
        } else {
            ui->raw->setPlainText( "[ERROR] Can\'t generate Raw String Delimiter " );
        }
    }
}


