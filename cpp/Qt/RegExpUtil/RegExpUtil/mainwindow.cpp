#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
      // SC_MatchOptCnt = 3
    , m_uiMatchOptWidgetAry{  qMakePair(nullptr,0u),  qMakePair(nullptr,0u), qMakePair(nullptr,0u), } 
      // SC_MatchTypeCnt  = 4
    , m_uiMatchTypeWidgetAry{  qMakePair(nullptr,0u), qMakePair(nullptr,0u), qMakePair(nullptr,0u), qMakePair(nullptr,0u), }
      // SC_PatternOptCnt = 10
    , m_uiPatternOptWidgetAry{ qMakePair(nullptr,0u), qMakePair(nullptr,0u), qMakePair(nullptr,0u), qMakePair(nullptr,0u), qMakePair(nullptr,0u), 
                               qMakePair(nullptr,0u), qMakePair(nullptr,0u), qMakePair(nullptr,0u), qMakePair(nullptr,0u), qMakePair(nullptr,0u),  
    }
{
    ui->setupUi(this);

    test_Study_perl_reg();

    init_WidgetAry();
    init_UI();
}

void MainWindow::init_WidgetAry()
{

    // SC_MatchOptCnt = 3  <Multi Choice(s)>
    m_uiMatchOptWidgetAry[0] = qMakePair( ui->mthOpt_NoMthOpt_chk,             static_cast<unsigned int>( QRegularExpression::NoMatchOption) );               
    m_uiMatchOptWidgetAry[1] = qMakePair( ui->mthOpt_AnchoredMthOpt_chk ,      static_cast<unsigned int>( QRegularExpression::AnchoredMatchOption) );
    m_uiMatchOptWidgetAry[2] = qMakePair( ui->mtchOpt_dontchkSubStrMthOpt_chk, static_cast<unsigned int>( QRegularExpression::DontCheckSubjectStringMatchOption) );

    // SC_MatchTypeCnt  = 4 <Single Choice>
    m_uiMatchTypeWidgetAry[0] = qMakePair( ui->mthTy_normalMth_rBtn,            static_cast<unsigned int>(QRegularExpression::NormalMatch) );
    m_uiMatchTypeWidgetAry[1] = qMakePair( ui->mthTy_partialPreCmpMth_rBtn,     static_cast<unsigned int>(QRegularExpression::PartialPreferCompleteMatch) );
    m_uiMatchTypeWidgetAry[2] = qMakePair( ui->mthTy_partPre1stMth_rBtn,        static_cast<unsigned int>(QRegularExpression::PartialPreferFirstMatch) );
    m_uiMatchTypeWidgetAry[3] = qMakePair( ui->mthTy_noMth_rBtn,                static_cast<unsigned int>(QRegularExpression::NoMatch) );

    // SC_PatternOptCnt = 10 <Multi Choice(s)>
    m_uiPatternOptWidgetAry[0] = qMakePair( ui->ptnOpt_0_chk,   static_cast<unsigned int>( QRegularExpression::NoPatternOption) );
    m_uiPatternOptWidgetAry[1] = qMakePair( ui->ptnOpt_1_chk,   static_cast<unsigned int>( QRegularExpression::CaseInsensitiveOption) );
    m_uiPatternOptWidgetAry[2] = qMakePair( ui->ptnOpt_2_chk,   static_cast<unsigned int>( QRegularExpression::DotMatchesEverythingOption) );
    m_uiPatternOptWidgetAry[3] = qMakePair( ui->ptnOpt_3_chk,   static_cast<unsigned int>( QRegularExpression::MultilineOption) );
    m_uiPatternOptWidgetAry[4] = qMakePair( ui->ptnOpt_4_chk,   static_cast<unsigned int>( QRegularExpression::ExtendedPatternSyntaxOption) );
    m_uiPatternOptWidgetAry[5] = qMakePair( ui->ptnOpt_5_chk,   static_cast<unsigned int>( QRegularExpression::InvertedGreedinessOption) );
    m_uiPatternOptWidgetAry[6] = qMakePair( ui->ptnOpt_6_chk,   static_cast<unsigned int>( QRegularExpression::DontCaptureOption) );
    m_uiPatternOptWidgetAry[7] = qMakePair( ui->ptnOpt_7_chk,   static_cast<unsigned int>( QRegularExpression::UseUnicodePropertiesOption) );
    m_uiPatternOptWidgetAry[8] = qMakePair( ui->ptnOpt_8_chk,   static_cast<unsigned int>( QRegularExpression::OptimizeOnFirstUsageOption) );
    m_uiPatternOptWidgetAry[9] = qMakePair( ui->ptnOpt_9_chk,   static_cast<unsigned int>( QRegularExpression::DontAutomaticallyOptimizeOption) );
}



void MainWindow::init_UI()
{
    // SC_MatchOptCnt = 3  <Multi Choice(s)> ( CheckBoxes )
    for( int i = 0; i < SC_MatchOptCnt; ++ i ) {
        m_uiMatchOptWidgetAry[i].first->setCheckState( i==0 ? Qt::Checked : Qt::Unchecked );
    }

    // SC_MatchTypeCnt  = 4 <Single Choice>  ( RadioButtons )
    for( int i = 0; i < SC_MatchTypeCnt; ++i ) {
        m_uiMatchTypeWidgetAry[i].first->setChecked( i==0 );
    }

    // SC_PatternOptCnt = 10 <Multi Choice(s)> ( CheckBoxes )
    for( int i = 0; i < SC_PatternOptCnt; ++i ) {
        m_uiPatternOptWidgetAry[i].first->setCheckState( i==0 ? Qt::Checked : Qt::Unchecked );
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_matchBtn_clicked()
{
    static int s_funcMatchCnt = 0;
    ++s_funcMatchCnt;

    qDebug() << "Match Button Click";

    QString inputPattern( ui->patternTxt->toPlainText() );
    QString cppRawString = QRegularExpression::escape( inputPattern );
    qDebug() << QString("Input re : %1\nRaw    re : %2").arg( inputPattern ).arg( cppRawString );

    auto patternOpts = this->getPatternOptions();
    QRegularExpression match_re( inputPattern , patternOpts );
    if ( !match_re.isValid() ) {
        QString errorStr = match_re.errorString();
        ui->statusbar->showMessage( errorStr );
        ui->resultTxt->setPlainText( errorStr );
        return;
    }

    int offset = 0;
    QRegularExpressionMatch matchRet = match_re.match( ui->mainTxt->toPlainText(),         // main string
                                    offset,
                                    this->getMatchType(),               // QRegularExpression::MatchType matchType = NormalMatch
                                    this->MainWindow::getMatchOptions() // QRegularExpression::MatchOptions matchOptions = NoMatchOption
                                   );

    ui->resultTxt->setPlainText( QString("[%1] %2.").arg( s_funcMatchCnt ).arg( matchRet.hasMatch() ? " Match :)" : " [NOT] Matched! "));
}

void MainWindow::on_globalMatchBtn_clicked()
{
    static int s_funcGlobalMatchCnt = 0;
    ++s_funcGlobalMatchCnt;

    qDebug() << "global Match Button Click";

    QString inputPattern( ui->patternTxt->toPlainText() );
    QString cppRawString = QRegularExpression::escape( inputPattern );
    qDebug() << QString("Input re : %1\nRaw    re : %2").arg( inputPattern ).arg( cppRawString );

    auto patternOpts = this->getPatternOptions();
    QRegularExpression match_re( inputPattern , patternOpts );
    if ( !match_re.isValid() ) {
        QString errorStr = match_re.errorString();
        ui->statusbar->showMessage( errorStr );
        ui->resultTxt->setPlainText( errorStr );
        return;
    }

    int offset = 0;
    QRegularExpressionMatch matchRet = match_re.match( ui->mainTxt->toPlainText(),         // main string
                                    offset,
                                    this->getMatchType(),               // QRegularExpression::MatchType matchType = NormalMatch
                                    this->MainWindow::getMatchOptions() // QRegularExpression::MatchOptions matchOptions = NoMatchOption
                                   );

    ui->resultTxt->setPlainText( QString("[%1] %2.").arg( s_funcGlobalMatchCnt ).arg( matchRet.hasMatch() ? " Match (global) :)" : " [NOT] Matched (global) ! "));
}

void MainWindow::on_anchoredBtn_clicked()
{
    qDebug() << "Anchored Button Click";
}

void MainWindow::on_escapleBtn_clicked()
{
    qDebug() << "Escaple Button Click";

}

void MainWindow::on_wildcardReBtn_clicked()
{
    qDebug() << "Wild_Card to RegularExpression Button Click";

}

void MainWindow::on_cppBtn1_clicked()
{
    auto bRunTestFlag = true;
    qDebug() << "Cpp#1 Button Click";
    if ( !bRunTestFlag ) {
        return;
    }


    QString mainText( ui->mainTxt->toPlainText() );
    QString pattern(  ui->patternTxt->toPlainText() ); (void)pattern;
    QString codePattern1( "Z\\" );
    QString codePattern2( "Z\\\\y" );
    qDebug() << "mainTxt = " << mainText;
    qDebug() << QString("pattern = %1 (%2), p1 = %3 (%4), p2 = %5 (%6)").arg(pattern).arg( QRegularExpression::escape(pattern) ).arg(codePattern1).arg( QRegularExpression::escape(codePattern1) ).arg(codePattern2).arg( QRegularExpression::escape(codePattern2) );

    QRegularExpression re1( codePattern1 ); 
    QRegularExpression re2( codePattern2 );
    if ( !re1.isValid() ) {
        qDebug() << "re1 is [NOT] Valid. " << re1.errorString();
    } else {
        QRegularExpressionMatch matchRet1 = re1.match( mainText );
        qDebug() << "m1 : " << ( matchRet1.hasMatch()  ? " Matched :)" : " Not Matched!!!" );
    }

    if ( !re2.isValid() ) {
        qDebug() << "re2 is [NOT] Valid. " << re2.errorString();
    } else {
        QRegularExpressionMatch matchRet2 = re2.match( mainText );
        qDebug() << "m2 : " << ( matchRet2.hasMatch()  ? " Matched :)" : " Not Matched!!!" );
    }

    

}

void MainWindow::on_checkPatternBtn_clicked()
{
    qDebug() << "Check Pattern Button Click";

    int chkCnt = 0;
    // SC_PatternOptCnt = 10 <Multi Choice(s)>
    for( int i = 0; i < SC_PatternOptCnt; ++i ) {
        chkCnt += ( (m_uiPatternOptWidgetAry[i].first->checkState() == Qt::Checked ) ? 1: 0);
    }

    if ( chkCnt == 0 || chkCnt == SC_PatternOptCnt   ) {
        ui->statusbar->showMessage(QStringLiteral("%1 [Pattern] options checked is [NOT] Allowed! ").arg(chkCnt == 0 ? "None" : "All " ), 3500);

        // reset as default
        for( int i = 0; i < SC_PatternOptCnt; ++i ) {
            m_uiPatternOptWidgetAry[i].first->setCheckState( i==0 ? Qt::Checked : Qt::Unchecked);
        }
    }

}

void MainWindow::on_matchValidBtn_clicked()
{
    qDebug() << "Check Match Option Valid Button Click";

    int chkCnt = 0;
    // SC_MatchOptCnt = 3
    for( int i = 0; i < SC_MatchOptCnt; ++i ) {
        chkCnt +=  (m_uiMatchOptWidgetAry[i].first->checkState() == Qt::Checked ? 1 : 0);
    }

    if ( chkCnt == 0 || chkCnt == SC_MatchOptCnt ) {
        ui->statusbar->showMessage( QString("%1 [Match] options checked is [NOT] Allowed! ").arg( chkCnt == 0 ? "None" : "All") , 3500);

        // reset as default
        for( int i = 0; i < SC_MatchOptCnt; ++i ) {
            m_uiMatchOptWidgetAry[i].first->setCheckState( i==0 ? Qt::Checked : Qt::Unchecked);
        }
    } 


}



QRegularExpression::MatchType    MainWindow::getMatchType()
{
    // Default Value : QRegularExpression::NormalMatch  
    // <QRadioButton>   Single Choice
    for(int i = 0; i < SC_MatchTypeCnt; ++i ) {
        if ( m_uiMatchTypeWidgetAry[i].first->isChecked() ) {
           return static_cast<QRegularExpression::MatchType>( m_uiMatchTypeWidgetAry[i].second  );
        }
    }

    return QRegularExpression::NormalMatch;
}

QRegularExpression::MatchOptions MainWindow::getMatchOptions()
{
    // Default Value : QRegularExpression::NoMatchOption
    // <QCheckBox>   Multi Choice(s)
    unsigned int flags = 0u;
    for(int i = 0; i < SC_MatchOptCnt; ++i ) {
        if ( m_uiMatchOptWidgetAry[i].first->checkState() == Qt::Checked ) {
           flags |= (m_uiMatchOptWidgetAry[i].second);
        }
    }

    return static_cast<QRegularExpression::MatchOptions>( flags ) ;
}



QRegularExpression::PatternOptions    MainWindow::getPatternOptions()
{
    unsigned int flags = 0u;
    for(int i = 0; i < SC_PatternOptCnt; ++i ) {
        if ( m_uiPatternOptWidgetAry[i].first->checkState() == Qt::Checked ) {
           flags |= (m_uiPatternOptWidgetAry[i].second);
        }
    }

    return static_cast<QRegularExpression::PatternOptions>( flags ) ;
}


void MainWindow::test_Study_case0()
{    
    // The word anchor \b matches a **[Boundary]** between a word character and a non-word character \w\W or \W\w      : 
    //                          [9,12)
    //           Matched |<Space>cat| inside catenates 
    // QString mainString("Housecat catenates house and cat");
    QString mainString("aaa");
    //                          [s^^)
    //                          

    QRegularExpression re("\\bcat");
    if ( !re.isValid() ) {
        qDebug() << "\\b is not valid inside a reg : " << re.errorString();
    } else {
        auto mthRet = re.match( mainString );
        qDebug() << "anchor [Boundary]  \\b" << (mthRet.hasMatch() ? " [[ Matched ]] :)" : " [[ NOT ]] Matched!! ");
        QString capturedstr = mthRet.captured(0);
        int beg_pos = mthRet.capturedStart(0); // position is included.
        int end_pos = mthRet.capturedEnd(0);   // position is excluded.
        qDebug() << "Captured = " << capturedstr << ", " <<  (mthRet.hasPartialMatch() ? " Partial Match " : " [NOT] Partial Match ");
        qDebug() << QString("1. beg = %1, end = %2").arg(beg_pos).arg(end_pos);
        qDebug() << "capturedTexts = " << mthRet.capturedTexts();
    }


}

void MainWindow::test_Study_case1()
{
}
    
void MainWindow::test_Study_perl_reg()
{
    // Boundary match
    test_Study_case0();

    test_Study_case1();

}



