#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QRandomGenerator> 
#include <QMessageBox>
#include <QFileDialog> 
#include <QFile> 
#include <QTextStream>

#include <QUrl>

// at most 5.0 second
static const double sc_LIMIT_TIME = 5.0;

// static const QString sc_HIGH_LIGHT_STYLE_SHEET(R"(
//  background-color: #EE82EE;
//  color: #00FF7F;
//  border: 8px solid green;
//  padding: 2px;
// )");

static const QString sc_HIGH_LIGHT_STYLE_SHEET(R"(
 background-color: #EE82EE;
 color: blue;
 border: 8px solid green;
 padding: 2px;
)");

static const QString sc_STATUS_BAR_SHEET_ERROR(R"(
 color: red;
)");

static const QString sc_STATUS_BAR_SHEET_CORRECT(R"(
 color: green;
)");


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_runningState( E_STOPPED )
    , m_cdTickInterval( 150 )
    , m_tickInterval(100) // 100 ms
    , m_tickCount(0)
    , m_maxQuestionCount( 100 )
    , m_questionIdx( 0 )
    , m_correctCnt(0)
    , m_incorrectCnt(0)
    , m_321goGif( new QMovie(":/img/321_Go.gif") )
    , m_countDownSoundIdx( 0 )
    , m_countDownSoundMaxCnt( 3 )
    , m_cdSoundFileList{ QString("qrc:///audio/secTick1.mp3"), QString("qrc:///audio/secTick2.mp3"), QString("qrc:///audio/c4boom.mp3") }
    , m_soundTag(0)
    , m_originalFontSize( 0 )
    , ms_opRandomPool{ "+" , "-", "×", "÷" }
    , ms_relopRandomPool{ "<", "⩽", ">", "⩾", "=", "==", "≠ ", "≈" }
    , m_currentAnswer()
    , m_currentModAnswer()
    , m_questionType( 0 )
    , m_bNeedInputMod( false )
    , m_pSoundPlayer(nullptr)
    , m_pEffectPlayer( nullptr )
    , m_pSettingDlg( nullptr )
{
    ui->setupUi(this);

    // m_tagStringMap.insert( static_cast<int>(E_ID_TAG), "序号" );
    m_tagStringMap.insert( static_cast<int>(E_QUESTION_TAG), "题目" );
    m_tagStringMap.insert( static_cast<int>(E_COUNT_DOWN_TAG), "计时" );
    m_tagStringMap.insert( static_cast<int>(E_YES_NO_UNDECIDE_TAG), "状态" ); // Yes/No/ Undecided
    m_tagStringMap.insert( static_cast<int>(E_ANSWER_TAG), "正确答案" );
    m_tagStringMap.insert( static_cast<int>(E_INPUT_ANSWER_TAG), "输入答案" );
    m_tagStringMap.insert( static_cast<int>(E_CORRECT_RATE_TAG), "正确率" );
    m_tagStringMap.insert( static_cast<int>(E_INCORRECT_RATE_TAG), "错误率" );

    // set sound audio player
    m_pSoundPlayer = new QMediaPlayer( this );
    m_pEffectPlayer = new QMediaPlayer( this );
    connect(m_pEffectPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::on_mediaStatusChanged );

    initUI();

    m_questionDuringTick.stop();
    connect(&m_questionDuringTick, SIGNAL(timeout()), this, SLOT(on_updateLCDNumber()) );

    m_countDownTimer.stop();
    connect(&m_countDownTimer, SIGNAL(timeout()), this, SLOT(on_randomQuestion()) );
    connect(m_321goGif, SIGNAL(finished()), this, SLOT(on_321goFinished()) );

    connect(ui->op1_Edit, SIGNAL(returnPressed()), this, SLOT( on_btnNextQuestion_clicked() ) );
    connect(ui->op2_Edit, SIGNAL(returnPressed()), this, SLOT( on_btnNextQuestion_clicked()) );
    connect(ui->result_Edit, SIGNAL(returnPressed()), this, SLOT( on_btnNextQuestion_clicked() ) );
    connect(ui->mod_Edit, SIGNAL(returnPressed()), this, SLOT( on_btnNextQuestion_clicked() ) );

    m_pSettingDlg = new Dialog;
    resetQuestionListByCfg( m_pSettingDlg->getCurrentCfg() );

}

MainWindow::~MainWindow()
{
    delete ui;

    delete m_321goGif;
    m_321goGif = nullptr;

    delete m_pSettingDlg;
    m_pSettingDlg = nullptr;

}

void MainWindow::initUI()
{
    ui->cdLabel->show();
    ui->cdLabel->setText("请点击菜单中的 开始 项 ... ");
    ui->lcdNumber->hide();

    ui->threePoints->hide();
    ui->mod->hide();
    ui->mod_Edit->hide();
    ui->mod_Edit->setText("");

    ui->op1_Edit->hide();
    ui->op2_Edit->hide();
    ui->result_Edit->hide();

    // ui->tableWidget->clear();
    QStringList labels;
    foreach ( int tag, m_tagStringMap.keys() ) {
       labels << m_tagStringMap.value(tag);
    }

    ui->tableWidget->setHorizontalHeaderLabels(labels);


    ui->tableWidget->setColumnWidth(static_cast<int>(E_QUESTION_TAG), 90);
    ui->tableWidget->setColumnWidth(static_cast<int>(E_YES_NO_UNDECIDE_TAG),64);
    ui->tableWidget->setColumnWidth(static_cast<int>(E_ANSWER_TAG), 88);
    ui->tableWidget->setColumnWidth(static_cast<int>(E_INPUT_ANSWER_TAG), 88);

    ui->tableWidget->setColumnWidth(static_cast<int>(E_CORRECT_RATE_TAG), 100);
    ui->tableWidget->setColumnWidth(static_cast<int>(E_INCORRECT_RATE_TAG), 100);

    ui->tableWidget->resizeColumnsToContents();

}

// TODO : modRestAnswer
void MainWindow::nextQuestion(bool correct, const QString& input, const QString& modRestAnswer /* = QString()*/ )
{
    if ( m_runningState != E_STARTED   ) {
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
        ui->statusbar->showMessage("请点击菜单中的 开始 项 ... ", 5000);
        return;
    }

    if (  m_countDownTimer.isActive() ) {
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
        ui->statusbar->showMessage( "无法切换下一题, 正在倒计时中 ...", 3000);
        return;
    }

    if ( m_questionIdx > 0 ) {
        if ( correct ) {
            ++m_correctCnt;
        } else {
            ++m_incorrectCnt;
        }
    }

    // update result of the current question
    if ( m_questionIdx <= m_maxQuestionCount ) {
        updateCurrentResultUI( correct,  input, modRestAnswer);
    }

    //
    ++m_questionIdx;
    if ( m_questionIdx > m_maxQuestionCount ) {
        ui->op1->setStyleSheet("");
        ui->op1_Edit->setStyleSheet("");
        ui->op2->setStyleSheet("");
        ui->op2_Edit->setStyleSheet("");
        ui->result->setStyleSheet("");
        ui->result_Edit->setStyleSheet("");
        ui->mod_Edit->setStyleSheet("");

        ui->cdLabel->show();
        ui->cdLabel->setText("全部题库已经完成");
        ui->lcdNumber->hide();

        on_action_stop_triggered();
        return;
    }

    // start gif animation
    m_questionDuringTick.stop();

    ui->op1_Edit->hide();
    ui->op1->show();
    ui->op1->setStyleSheet("");
    ui->op1_Edit->setStyleSheet("");

    ui->op2_Edit->hide();
    ui->op2->show();
    ui->op2->setStyleSheet("");
    ui->op2_Edit->setStyleSheet("");

    ui->result_Edit->hide();
    ui->result->show();
    ui->result->setStyleSheet("");
    ui->result_Edit->setStyleSheet("");

    ui->threePoints->hide();
    ui->mod->hide();
    ui->mod_Edit->hide();

    // 3-2-1 Go CountDown Start
    m_countDownTimer.setTimerType(Qt::PreciseTimer);
    m_countDownTimer.start(m_cdTickInterval);

    ui->action_pause->setEnabled(false);
    ui->action_stop->setEnabled(false);

    ui->lcdNumber->display("0.00");
    ui->lcdNumber->hide();

    ui->cdLabel->show();
    ui->cdLabel->setMovie( m_321goGif );
    m_321goGif->start();
 
    // ++m_countDownSoundIdx;
    // m_countDownSoundIdx = m_countDownSoundIdx % m_countDownSoundMaxCnt;

    m_soundTag = 0;
    m_pSoundPlayer->setMedia(QUrl("qrc:///audio/3_2_1_Go.mp3"));
    m_pSoundPlayer->setVolume(50);
    m_pSoundPlayer->play();
}


/*
// virtual
void MainWindow::keyReleaseEvent(QKeyEvent *event) // Q_DECL_OVERRIDE;
{
    if ( event!=nullptr ) {
        auto keycode = event->key();
        if ( keycode == Qt::Key_Space ) {
            nextQuestion(true);
            // qDebug() << "<Space> Released";
        } else {
            nextQuestion(false);
            // qDebug() << "Others " << keycode << " Released";
        }
    }
}
*/



void MainWindow::on_action_start_triggered()
{
    if ( m_runningState != E_STOPPED ) {
        return;
    }

    resetQuestionListByCfg( m_pSettingDlg->getCurrentCfg() );
    if ( m_maxQuestionCount <= 0 ) {
        const QString msg("题库为空, 无法启动 !  :)");
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
        ui->statusbar->showMessage( msg, 3000);
        return;
    }



    m_questionIdx = 0;
    m_correctCnt = 0;
    m_incorrectCnt = 0;
    m_runningState = E_STARTED;
    m_currentAnswer = "";
    m_questionType = 0;
    m_bNeedInputMod = false;

    ui->action_start->setEnabled(false);
    ui->action_pause->setEnabled(true);
    ui->action_stop->setEnabled(true);
    ui->action_settings->setEnabled(false);

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->resizeColumnsToContents();

    nextQuestion(true, "");
}

void MainWindow::on_action_pause_triggered()
{
    if ( m_runningState != E_STARTED && m_runningState != E_PAUSED  ) {
        return;
    }

    m_runningState = ((m_runningState == E_STARTED) ? E_PAUSED : E_STARTED);
    if ( m_runningState == E_PAUSED ) {
        m_tickCount = m_tickCount + m_elpTimer.elapsed();
        m_questionDuringTick.stop();

        ui->action_settings->setEnabled(false);
    } else {
        // resume
        m_questionDuringTick.setTimerType(Qt::PreciseTimer);
        m_questionDuringTick.start(m_tickInterval);
        m_elpTimer.restart();
    }


    ui->action_start->setEnabled(false);
    ui->action_pause->setEnabled(true);
    ui->action_pause->setText( tr(m_runningState==E_STARTED ? "暂停" : "继续") );
    ui->action_stop->setEnabled(true);

    ui->action_settings->setEnabled(true);
}

void MainWindow::on_action_stop_triggered()
{
    if ( m_runningState != E_STARTED ) {
        return;
    }

    m_runningState = E_STOPPED;
    m_tickCount = 0;
    m_questionDuringTick.stop();

    ui->action_start->setEnabled(true);
    ui->action_pause->setEnabled(false);
    ui->action_pause->setText(tr("暂停") );
    ui->action_stop->setEnabled(false);

    ui->action_settings->setEnabled(true);
}

void MainWindow::on_action_settings_triggered()
{
    m_pSettingDlg->reset_ui( m_pSettingDlg->getCurrentCfg() );
    auto ret = m_pSettingDlg->exec();
    if ( ret != 0 ) {
        resetQuestionListByCfg( m_pSettingDlg->getCurrentCfg() );

        const QString msg("题库已经按照新的选项进行设置 :)");
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_CORRECT );
        ui->statusbar->showMessage( msg, 3000);
        return;
    }

}

void MainWindow::on_action_save_triggered()
{
    const QString END_LINE("\n");
    const QString VLINE("|");

    if ( m_questionIdx == 0 && m_runningState == E_STOPPED ) {
        const QString msg("测试未开始，无数据需要被保存");
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
        ui->statusbar->showMessage( msg, 3000);

        QMessageBox::information(this, "保存测试结果", msg);
        return;
    }

    QString exePath = qGuiApp->applicationDirPath();
    auto saveFileName = QFileDialog::getSaveFileName( this, "保存[本次测试结果]", exePath, "Text files (*.txt);;" );
    if ( saveFileName.isEmpty() ) {
        QMessageBox::information(this, "保存测试结果", "文件名不能为空");
        return;
    }

    QString filled;
    QString fileContent;
    // qDebug() << "saveFileName = " << saveFileName;

    fileContent += VLINE;
    fileContent += ( QString("#").leftJustified(6)     + VLINE  ); //  -1
    fileContent += ( QString("Question").leftJustified(25)    + VLINE  ); //  0: E_QUESTION_TAG
    fileContent += ( QString("Time").leftJustified(8)     + VLINE  ); //  1: E_COUNT_DOWN_TAG
    fileContent += ( QString("State").leftJustified(6)     + VLINE  ); //  2: E_YES_NO_UNDECIDE_TAG
    fileContent += ( QString("Answer").leftJustified(6)     + VLINE  ); //  3: E_ANSWER_TAG
    fileContent += ( QString("Input").leftJustified(6)     + VLINE  ); //  4: E_INPUT_ANSWER_TAG
    fileContent += ( QString("Correct Rate").leftJustified(12)  + VLINE  ); //  5: E_CORRECT_RATE_TAG
    fileContent += ( QString("Error Rate").leftJustified(12)  + VLINE  ); //  6: E_INCORRECT_RATE_TAG
    fileContent += END_LINE;
    fileContent += ( filled.fill( QChar('-'), 100) + END_LINE );
                             
    for( auto i = 0; i < m_questionIdx; ++i ) {
        fileContent += "|";
        QString idtag = QString("%1").arg( i+1 ); // question id
        fileContent += ( idtag.leftJustified(6) + VLINE );

        for( auto j = static_cast<int>(E_QUESTION_TAG); j < E_TAG_MAX; ++j ) { 
            auto item = ui->tableWidget->item(i, static_cast<int>(j) );
            if ( item!=nullptr ) {
                QString content;
                // qDebug() << i << " , " <<  j << ".  c => " <<  item->icon().name();
                switch( j )
                {
                case E_QUESTION_TAG:
                    {
                        content = item->text();
                        fileContent += (content.leftJustified(25) + VLINE);
                    }
                    break;
                case E_COUNT_DOWN_TAG:
                    {
                        content = item->text();
                        fileContent += (content.leftJustified(8) + VLINE);
                    }
                    break;
                case E_YES_NO_UNDECIDE_TAG:
                    {
                        content = item->data( Qt::UserRole+1 ).toString();
                        fileContent += (content.leftJustified(6) + VLINE);
                    }
                    break;
                case E_ANSWER_TAG:
                    {
                        content = item->text();
                        fileContent += (content.leftJustified(6) + VLINE);
                    }
                    break;
                case E_INPUT_ANSWER_TAG:
                    {
                        content = item->text();
                        fileContent += (content.leftJustified(6) + VLINE);
                    }
                    break;
                case E_CORRECT_RATE_TAG:
                    {
                        content = item->text();
                        fileContent += (content.leftJustified(12) + VLINE);
                    }
                    break;
                case E_INCORRECT_RATE_TAG:
                    {
                        content = item->text();
                        fileContent += (content.leftJustified(12) + VLINE);
                    }
                    break;
                default:
                    break;
                }
            }

        }
        fileContent += END_LINE;
    }
    fileContent += END_LINE;

    QFile file( saveFileName );
    if ( !file.open(QIODevice::WriteOnly | QIODevice::Text) ) {
        const QString msg("保存文件失败");
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
        ui->statusbar->showMessage( msg, 3000);

        QMessageBox::information(this, "保存测试结果", msg);
        return;
    }

    QTextStream outstream(&file);
    outstream << fileContent;
    file.flush();
    file.close();

    const QString msgOK("文件保存成功");
    ui->statusbar->clearMessage();
    ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_CORRECT );
    ui->statusbar->showMessage( msgOK, 3000);

    QMessageBox::information(this, "保存文件", msgOK);
}

void MainWindow::on_action_load_triggered()
{
    QMessageBox::information(this, "Settings" ,"此功能 正在开发中 ... , 敬请期待");
}

void MainWindow::on_updateLCDNumber()
{
    // qDebug() << "Update : ";
    auto total  = m_tickCount + m_elpTimer.elapsed(); // in ms
    double realtime = total / 1000.0;

    QString strNumber = QString("%1").arg(realtime,5, 'f', 2);
    ui->lcdNumber->display( strNumber );


    // Update elapsed time inside table
    int currentQid = 0;
    if ( m_questionIdx > 0 ) {
        currentQid = m_questionIdx-1;
    }
    auto timerItem = ui->tableWidget->item(currentQid, static_cast<int>(E_COUNT_DOWN_TAG) );
    if ( timerItem!=nullptr ) {
        timerItem->setText( strNumber );
        if ( realtime >= sc_LIMIT_TIME  ) {
            timerItem->setBackground( QBrush( Qt::black ) );
            timerItem->setForeground( QBrush( Qt::red ) );
            auto oldFont = timerItem->font();
            timerItem->setFont( QFont( oldFont.family(),  m_originalFontSize + 15, QFont::ExtraBold ) );
            
            auto pickedsound_info = m_cdSoundFileList.at(m_countDownSoundIdx); 

            auto playedState = m_pEffectPlayer->state();
            if ( playedState == QMediaPlayer::StoppedState ) {
                qDebug() << "Play " << pickedsound_info;

                ui->tableWidget->resizeColumnsToContents();

                m_pEffectPlayer->setMedia(QUrl( pickedsound_info ));
                m_pEffectPlayer->setVolume(80);
                m_pEffectPlayer->play();

                // loop + 1
                ++m_countDownSoundIdx;
                m_countDownSoundIdx = m_countDownSoundIdx % m_countDownSoundMaxCnt;
            }

            m_soundTag = 2;
        } else {
            timerItem->setForeground( QBrush( Qt::green ) );
        }
    }
}



void MainWindow::on_321goFinished()
{
    // qDebug() << "GIF Movie has played at the end";
    auto ranMachine = QRandomGenerator::global();

    ui->cdLabel->hide();
    ui->lcdNumber->show();

    m_countDownTimer.stop();

    m_tickCount = 0;
    m_questionDuringTick.setTimerType(Qt::PreciseTimer);
    m_questionDuringTick.start(m_tickInterval);
    m_elpTimer.start();

    m_321goGif->stop();


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Set Question UI Labels
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ui->eq->setText("=");

    //
    // Set Operator
    //
    if ( m_4opPossibilities.empty() ) {
        m_4opPossibilities = m_4opPossibilitiesBackup;
    } 
    int opType = 0;
    int opSz = m_4opPossibilities.size();
    if ( opSz == 1 ) {
        opType = m_4opPossibilities.takeAt(0);
    } else {
        opType = m_4opPossibilities.takeAt(  ranMachine->bounded(0,opSz ) );
    }
    ui->op->setText( ms_opRandomPool[opType] );



    if ( m_3opPossibilities[opType].empty() ) {
        m_3opPossibilities[opType] = m_3opPossibilitiesBackup[opType];
    }

    int rType = 0;
    int rSz = m_3opPossibilities[opType].size();
    if ( rSz == 1 ) {
        rType = m_3opPossibilities[opType].takeAt(0);
    } else {
        rType = m_3opPossibilities[opType].takeAt( ranMachine->bounded(0,rSz) );
    }

    //
    // Set Op1 , Op2 , Result
    //
    QPair<int,int> questionPr;

    int qSz = 0;
    int res = 0;
    int mod_rest = 0;
    // pick pair into questionList
    if ( opType == 0 ) {
        if ( m_plusQuestionList.empty() ) {
            m_plusQuestionList = m_plusQuestionListBackup;
        }

        qSz = m_plusQuestionList.size();
        if ( qSz == 1 ) {
            questionPr = m_plusQuestionList.takeAt(0);
        } else {
            questionPr = m_plusQuestionList.takeAt( ranMachine->bounded(0,qSz) );
        }
        res = questionPr.first + questionPr.second;
    } else if ( opType == 1 ) {
        // -
        if ( m_minusQuestionList.empty() ) {
            m_minusQuestionList  = m_minusQuestionListBackup;
        }

        qSz = m_minusQuestionList.size();
        if ( qSz == 1 ) {
            questionPr = m_minusQuestionList.takeAt(0);
        } else {
            questionPr = m_minusQuestionList.takeAt( ranMachine->bounded(0,qSz) );
        }
        res = questionPr.first - questionPr.second;
    } else if ( opType == 2 ) {
        // *
        if ( m_multiplyQuestionList.empty() ) {
             m_multiplyQuestionList = m_multiplyQuestionListBackup;
        }

        qSz = m_multiplyQuestionList.size();
        if ( qSz == 1 ) {
            questionPr = m_multiplyQuestionList.takeAt(0);
        } else {
            questionPr = m_multiplyQuestionList.takeAt( ranMachine->bounded(0,qSz) );
        }
        res = questionPr.first * questionPr.second;
    } else {
        // opType == 3 , divide
        if ( m_divideQuestionList.empty() ) {
             m_divideQuestionList = m_divideQuestionListBackup;
        }

        qSz = m_divideQuestionList.size();
        if ( qSz == 1 ) {
            questionPr = m_divideQuestionList.takeAt(0);
        } else {
            questionPr = m_divideQuestionList.takeAt( ranMachine->bounded(0,qSz) );
        }
        res = questionPr.first / questionPr.second;
        mod_rest =  questionPr.first % questionPr.second;
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // set final question content
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    m_currentModAnswer = "";
    m_questionType = rType;
    m_bNeedInputMod = false;
    if ( rType == 0 ) {
        // op1 is the question
        ui->op1_Edit->setStyleSheet( sc_HIGH_LIGHT_STYLE_SHEET );
        ui->op1_Edit->show();
        ui->op1_Edit->clear();
        ui->op1_Edit->setFocus();

        ui->op1->hide();
        // ui->op1->setStyleSheet( sc_HIGH_LIGHT_STYLE_SHEET );
        ui->op1->setText( "?" );
        m_currentAnswer = QString("%1").arg(questionPr.first);

        ui->op2_Edit->hide();
        ui->op2_Edit->setStyleSheet("");
        ui->op2->show();
        ui->op2->setStyleSheet("");
        ui->op2->setText( QString("%1").arg(questionPr.second) );

        ui->result_Edit->hide();
        ui->result_Edit->setStyleSheet("");
        ui->result->show();
        ui->result->setStyleSheet("");
        ui->result->setText( QString("%1").arg(res) );

        ui->mod_Edit->setStyleSheet("");
        ui->mod_Edit->setText("");

        if( opType == 3  ) {
            ui->mod_Edit->hide();
            ui->mod_Edit->setStyleSheet("");

            if ( mod_rest!=0 ) {
                ui->threePoints->show();

                ui->mod->show();
                ui->mod->setStyleSheet( "" );
                ui->mod->setText( QString("%1").arg(mod_rest) );

                m_currentModAnswer.setNum(mod_rest);
            } else {
                ui->threePoints->hide();

                ui->mod->hide();
                ui->mod->setText( "" );

                m_currentModAnswer.setNum(0);
            }
        } else { m_currentModAnswer = ""; } 
    } else if ( rType == 1 ) {
        // op1
        ui->op1_Edit->hide();
        ui->op1_Edit->setStyleSheet("");
        ui->op1->show();
        ui->op1->setStyleSheet("");
        ui->op1->setText( QString("%1").arg(questionPr.first) );

        // op2 is the question
        ui->op2_Edit->setStyleSheet( sc_HIGH_LIGHT_STYLE_SHEET );
        ui->op2_Edit->show();
        ui->op2_Edit->clear();
        ui->op2_Edit->setFocus();
        ui->op2->hide();
        // ui->op2->setStyleSheet( sc_HIGH_LIGHT_STYLE_SHEET );
        ui->op2->setText( "?" );
        m_currentAnswer = QString("%1").arg(questionPr.second);

        // result 
        ui->result_Edit->hide();
        ui->result_Edit->setStyleSheet("");
        ui->result->show();
        ui->result->setStyleSheet("");
        ui->result->setText( QString("%1").arg(res) );

        ui->mod_Edit->setStyleSheet("");
        ui->mod_Edit->setText("");

        if( opType == 3  ) {
            ui->mod_Edit->hide();
            ui->mod_Edit->setStyleSheet("");

            if ( mod_rest!=0 ) {
                ui->threePoints->show();

                ui->mod->show();
                ui->mod->setStyleSheet( "" );
                ui->mod->setText(  QString("%1").arg(mod_rest)  );

                m_currentModAnswer.setNum(mod_rest);
            } else {
                ui->threePoints->hide();
                ui->mod->hide();
                ui->mod->setText( "" );

                m_currentModAnswer.setNum(0);
            }
        } else { m_currentModAnswer = ""; }
    } else {
        ui->op1_Edit->hide();
        ui->op1_Edit->setStyleSheet("");
        ui->op1->show();
        ui->op1->setStyleSheet("");
        ui->op1->setText( QString("%1").arg(questionPr.first) );

        ui->op2_Edit->hide();
        ui->op2_Edit->setStyleSheet("");
        ui->op2->show();
        ui->op2->setStyleSheet("");
        ui->op2->setText( QString("%1").arg(questionPr.second) );

        // result is the question
        ui->result_Edit->setStyleSheet( sc_HIGH_LIGHT_STYLE_SHEET );
        ui->result_Edit->show();
        ui->result_Edit->clear();
        ui->result_Edit->setFocus();

        ui->mod_Edit->setStyleSheet("");
        ui->mod_Edit->setText("");

        ui->result->hide();
        // ui->result->setStyleSheet( sc_HIGH_LIGHT_STYLE_SHEET );
        ui->result->setText( "?" );
        m_currentAnswer = QString("%1").arg(res);

        if( opType == 3  ) {
            if ( mod_rest!=0 ) {
                ui->threePoints->show();
                ui->mod_Edit->show( );
                ui->mod_Edit->setStyleSheet( sc_HIGH_LIGHT_STYLE_SHEET );

                ui->mod->hide();
                ui->mod->setStyleSheet( sc_HIGH_LIGHT_STYLE_SHEET );
                ui->mod->setText( "?" );

                m_bNeedInputMod = true;
                m_currentModAnswer.setNum(mod_rest);
            } else {
                ui->threePoints->hide();
                ui->mod->hide();
                ui->mod->setText( "" );

                ui->mod_Edit->hide( );
                ui->mod_Edit->setStyleSheet( "" );

                m_currentModAnswer.setNum(0);
            }
        } else { m_currentModAnswer = ""; } 
    }

    int currentQid = 0;
    if ( m_questionIdx > 0 ) {
        currentQid = m_questionIdx-1;
    }
    ui->tableWidget->insertRow(currentQid);


    // set question
    {

        QString questionContent;
        if( opType == 3 ) {
            if ( mod_rest!=0 ) {
                questionContent = QString(" %1  %2  %3  =  %4 ... %5").arg( ui->op1->text() )
                                                                        .arg( ui->op->text() )
                                                                        .arg( ui->op2->text() )
                                                                        .arg( ui->result->text() )
                                                                        .arg( ui->mod->text() );
            } else {
                questionContent = QString(" %1  %2  %3  =  %4 ").arg( ui->op1->text() )
                                                                        .arg( ui->op->text() )
                                                                        .arg( ui->op2->text() )
                                                                        .arg( ui->result->text() );
            } 
        } else {
            questionContent = QString(" %1  %2  %3  =  %4 ").arg( ui->op1->text() )
                                                                    .arg( ui->op->text() )
                                                                    .arg( ui->op2->text() )
                                                                    .arg( ui->result->text() );

        }


        QTableWidgetItem *newItem = new QTableWidgetItem( questionContent );
        ui->tableWidget->setItem(currentQid , static_cast<int>(E_QUESTION_TAG), newItem);
    }

    // timer
    {
        QTableWidgetItem *newItem = new QTableWidgetItem( "0.00" );
        ui->tableWidget->setItem(currentQid , static_cast<int>(E_COUNT_DOWN_TAG), newItem);
        m_originalFontSize = newItem->font().pointSize();
    }

    // yes/no/ haven't decided yet
    {
        QTableWidgetItem *newItem = new QTableWidgetItem( QIcon(":/img/undecided.png"), "" );
        newItem->setData( (Qt::UserRole+1) , QVariant(QString("N/A")) );
        ui->tableWidget->setItem(currentQid , static_cast<int>(E_YES_NO_UNDECIDE_TAG), newItem);
    }

    // answer
    {
        QTableWidgetItem *newItem = new QTableWidgetItem((opType==3 && mod_rest!=0) ?  "? ... ?" : "?" );
        ui->tableWidget->setItem(currentQid , static_cast<int>(E_ANSWER_TAG), newItem);
    }

    // your input
    {
        QTableWidgetItem *newItem = new QTableWidgetItem("?");
        ui->tableWidget->setItem(currentQid , static_cast<int>(E_INPUT_ANSWER_TAG), newItem);
    }

    // correct rate
    {
        QTableWidgetItem *newItem = new QTableWidgetItem( "" );
        ui->tableWidget->setItem(currentQid , static_cast<int>(E_CORRECT_RATE_TAG), newItem);
    }

    // incorrect rate
    {
        QTableWidgetItem *newItem = new QTableWidgetItem( "" );
        ui->tableWidget->setItem(currentQid , static_cast<int>(E_INCORRECT_RATE_TAG), newItem);
        // ui->tableWidget->scrollToItem( newItem , QAbstractItemView::PositionAtCenter);
    }
    ui->tableWidget->scrollToBottom();
    ui->tableWidget->resizeColumnsToContents();

    ui->action_pause->setEnabled(true);
    ui->action_stop->setEnabled(true);
}

void MainWindow::on_randomQuestion()
{
    auto ranMachine = QRandomGenerator::global();

    ui->op1->setText( QString("%1").arg( ranMachine->bounded(10,100) ) );   // => [10,99]
    ui->op->setText( ms_opRandomPool[ ranMachine->bounded(0,OP_COUNT) ] );
    ui->op2->setText( QString("%1").arg( ranMachine->bounded(10,100) ) );  // => [10,99] 

    ui->eq->setText( ms_relopRandomPool[ ranMachine->bounded(0,RELOP_CNT) ] ); // < >  <=  >=  ...
    ui->result->setText( QString("%1").arg( ranMachine->bounded(10,100) ) );  // => [10,99]

}


void MainWindow::updateCurrentResultUI(bool correct, const QString& inputAnswer, const QString& inputModRestAnswer /* = QString()*/)
{
    if ( m_questionIdx == 0 ) {
        return;
    }

    int currentQid = m_questionIdx-1;
    QString questionInfo;
    {
        auto item = ui->tableWidget->item(currentQid, static_cast<int>(E_QUESTION_TAG) );
        questionInfo = item->text();
    }

    // yes/no/ haven't decided yet
    {
        auto item = ui->tableWidget->item(currentQid, static_cast<int>(E_YES_NO_UNDECIDE_TAG) );
        if ( item != nullptr ) {
            item->setData( (Qt::UserRole+1) , QVariant(correct ? QString("YES") : QString("NO")) );
            item->setIcon( QIcon(correct ? ":/img/yes.png" : ":/img/no.png") );
        }

        m_soundTag = 1;
        m_pEffectPlayer->setMedia(QUrl(correct ? "qrc:///audio/correct.mp3" : "qrc:///audio/incorrect.mp3"));
        m_pEffectPlayer->setVolume(50);
        m_pEffectPlayer->play();
        // m_pSoundPlayer->setMedia(QUrl(correct ? "qrc:///audio/correct.mp3" : "qrc:///audio/incorrect.mp3"));
        // m_pSoundPlayer->setVolume(50);
        // m_pSoundPlayer->play();
    }

    // correct answer
    {
        auto item = ui->tableWidget->item(currentQid, static_cast<int>(E_ANSWER_TAG) );
        if ( item != nullptr ) {
            // inputModRestAnswer
            if ( m_questionType!=2 ) {
                item->setText( m_currentAnswer );
            } else {
                if ( questionInfo.contains("÷") && m_currentModAnswer.toInt()!=0 ) {
                    QString correct_modAnswer = QString("%1  ... %2").arg( m_currentAnswer ).arg( m_currentModAnswer );
                    item->setText( correct_modAnswer );
                } else {
                    item->setText( m_currentAnswer );
                }
            }

        }
    }

    // your input answer
    {
        auto item = ui->tableWidget->item(currentQid, static_cast<int>(E_INPUT_ANSWER_TAG) );
        if ( item != nullptr ) {
            if ( m_bNeedInputMod ) {
                QString modAnswer = QString("%1  ... %2").arg(inputAnswer).arg( inputModRestAnswer );
                item->setText( modAnswer );
            } else {
                item->setText( inputAnswer );
            }

            item->setForeground( QBrush( QColor(correct ? Qt::green : Qt::red) ) );
        }
    }


    // correct rate
    {
        auto item = ui->tableWidget->item(currentQid, static_cast<int>(E_CORRECT_RATE_TAG) );
        if ( item != nullptr ) {
            double aRate = (m_correctCnt * 1.0 / (currentQid+1) * 100.0);
            QString str_aRate = QString("%1").arg(aRate,5, 'f', 2);
            item->setText(  QString("%1/%2 = %3%").arg( m_correctCnt ).arg( currentQid+1 ).arg( str_aRate ) );
        }
    }

    // incorrect rate
    {

        auto item = ui->tableWidget->item(currentQid, static_cast<int>(E_INCORRECT_RATE_TAG) );
        if ( item != nullptr ) {
            double eRate =  m_incorrectCnt * 1.0 / (currentQid+1) * 100.0;
            QString str_eRate = QString("%1").arg(eRate,5, 'f', 2);
            item->setText(  QString("%1/%2 = %3%").arg( m_incorrectCnt ).arg( currentQid+1 ).arg( str_eRate ) );
        }
    }


    {
        auto timerItem = ui->tableWidget->item(currentQid, static_cast<int>(E_COUNT_DOWN_TAG) );
        if ( timerItem!=nullptr ) {
            ui->statusbar->clearMessage();
            ui->statusbar->setStyleSheet( correct ? sc_STATUS_BAR_SHEET_CORRECT : sc_STATUS_BAR_SHEET_ERROR );
            ui->statusbar->showMessage( QString("第 %1 题用时 : %2 秒").arg(currentQid+1).arg( timerItem->text() ), 1000);

            if ( !correct ) {
                timerItem->setBackground( QBrush( Qt::black ) );
                timerItem->setForeground( QBrush( Qt::red ) );

                auto oldFont = timerItem->font();
                timerItem->setFont( QFont( oldFont.family(),  m_originalFontSize + 15, QFont::ExtraBold ) );
            }
        }
    }

    ui->tableWidget->resizeColumnsToContents();

}


void MainWindow::on_btnNextQuestion_clicked()
{
    if ( m_runningState != E_STARTED   ) {
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
        ui->statusbar->showMessage("请点击菜单中的 开始 项 ... ", 5000);
        return;
    }

    if (  m_countDownTimer.isActive() ) {
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
        ui->statusbar->showMessage( "无法切换下一题, 正在倒计时中 ...", 3000);
        return;
    }

    if ( m_questionIdx == 0 ) {
        return;
    }

    auto bConvertFlag = false;
    int cAnswer = m_currentAnswer.toInt(&bConvertFlag,10);
    if ( !bConvertFlag ) {
        ui->statusbar->clearMessage();
        ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
        ui->statusbar->showMessage( "1. 内部字符串-->整数转换错误", 3000);
        return;
    }
    
    // 
    int cModAnswer = 0;
    if ( !m_currentModAnswer.isEmpty() ) {
        cModAnswer = m_currentModAnswer.toInt(&bConvertFlag, 10);
        if ( !bConvertFlag ) {
            ui->statusbar->clearMessage();
            ui->statusbar->setStyleSheet( sc_STATUS_BAR_SHEET_ERROR );
            ui->statusbar->showMessage( "1. 余数中 内部字符串-->整数转换错误", 3000);
            return;
        }
    }


    QString modInputText;
    QString inputText;
    if ( m_questionType == 0 ) {
        inputText = ui->op1_Edit->text();
    } else if ( m_questionType == 1 ) {
        inputText = ui->op2_Edit->text();
    } else {
        inputText = ui->result_Edit->text();
        if ( !m_currentModAnswer.isEmpty() ) { // correct answer : m_currentModAnswer     ->  "0" or   none-zero
            modInputText = ui->mod_Edit->text();
        }
    }

    bConvertFlag = false;
    int inputAnswer = inputText.toInt(&bConvertFlag,10);
    if ( !bConvertFlag ) {
        inputAnswer = -1;
    }

    int i_inputModAnswer = 0; 
    if ( !m_currentModAnswer.isEmpty() ) {
        if ( modInputText.isEmpty() ) {
            i_inputModAnswer = 0;
        } else {
            bConvertFlag = false;
            i_inputModAnswer = modInputText.toInt(&bConvertFlag,10);
            if ( !bConvertFlag ) {
                i_inputModAnswer  = -1;
            }
        }
    }


    auto correct = (cAnswer == inputAnswer); 
    if ( m_bNeedInputMod ) {
        correct = correct && (cModAnswer == i_inputModAnswer);
    }

    // Start Next Question
    nextQuestion(correct, inputText, modInputText);
}


void  MainWindow::resetQuestionListByCfg(const Dialog::settingInfo& cfg)
{
    //
    // fill questions into Question-Pool
    //
    m_plusQuestionList.clear();
    m_plusQuestionListBackup.clear();
    m_minusQuestionList.clear();
    m_minusQuestionListBackup.clear();

    m_multiplyQuestionList.clear();
    m_multiplyQuestionListBackup.clear();
    m_divideQuestionList.clear();
    m_divideQuestionListBackup.clear();

    for( auto i = 0; i < OP_COUNT; ++i ) {
        m_3opPossibilities[i].clear();
        m_3opPossibilitiesBackup[i].clear();
    }

    m_4opPossibilities.clear();
    m_4opPossibilitiesBackup.clear();

    auto ranMachine = QRandomGenerator::global();
    if ( cfg.bIsAddGrpEnabled && cfg.iAddQuestionsCnt > 0 ) {
        auto genCnt = 0;
        for(     auto a = cfg.iAdd1RangeMinUIValue; a <= cfg.iAdd1RangeMaxUIValue; ++a ) {
            for( auto b = cfg.iAdd2RangeMinUIValue; b <= cfg.iAdd2RangeMaxUIValue; ++b ) {
                // auto sum = a + b;
                ++genCnt;
                m_plusQuestionList.push_back( qMakePair(a,b) );
                m_plusQuestionListBackup.push_back( qMakePair(a,b) );
            }
        }

        if ( genCnt < cfg.iAddQuestionsCnt ) {
            auto delta =  genCnt - cfg.iAddQuestionsCnt;
            for( auto i = 0; i < delta; ++i ) {
                auto ranQuestionPr =  m_plusQuestionList.at( ranMachine->bounded(0, m_plusQuestionList.size() ) );
                m_plusQuestionList.push_back( ranQuestionPr );
                m_plusQuestionListBackup.push_back( ranQuestionPr );
            }
        }
    }

    if ( cfg.bIsMinusGrpEnabled && cfg.iMinusQuestionsCnt > 0 ) {
        auto genCnt = 0;
        for(     auto a = cfg.iMinus1RangeMinUIValue; a <=cfg.iMinus1RangeMaxUIValue; ++a ) {
            for( auto b = cfg.iMinus2RangeMinUIValue; b <=cfg.iMinus2RangeMaxUIValue; ++b ) {
                auto diffvalue = a - b;
                if ( diffvalue >=0 || cfg.bIsResultNegativeEnabled ) {
                    ++genCnt;
                    m_minusQuestionList.push_back( qMakePair(a,b) );
                    m_minusQuestionListBackup.push_back( qMakePair(a,b) );
                } 
            }
        }

        if ( genCnt < cfg.iMinusQuestionsCnt ) {
            auto delta =  genCnt - cfg.iMinusQuestionsCnt;
            for( auto i = 0; i < delta; ++i ) {
                auto ranQuestionPr =  m_minusQuestionList.at( ranMachine->bounded(0, m_minusQuestionList.size() ) );
                m_minusQuestionList.push_back( ranQuestionPr );
                m_minusQuestionListBackup.push_back( ranQuestionPr );
            }
        }
    }

    if ( cfg.bIsMultiplyGrpEnabled && cfg.iMultiplyQuestionsCnt > 0 ) {
        auto genCnt = 0;
        for(     auto a = cfg.iMultiply1RangeMinUIValue;  a <= cfg.iMultiply1RangeMaxUIValue; ++a ) {
            for( auto b = cfg.iMultiply2RangeMinUIValue;  b <= cfg.iMultiply2RangeMaxUIValue; ++b ) {
                // auto product = a * b;
                ++genCnt;
                m_multiplyQuestionList.push_back( qMakePair(a,b) );
                m_multiplyQuestionListBackup.push_back( qMakePair(a,b) );
            }
        }

        if ( genCnt < cfg.iMultiplyQuestionsCnt ) {
            auto delta =  genCnt - cfg.iMultiplyQuestionsCnt;
            for( auto i = 0; i < delta; ++i ) {
                auto ranQuestionPr = m_multiplyQuestionList.at( ranMachine->bounded(0, m_multiplyQuestionList.size() ) );
                m_multiplyQuestionList.push_back( ranQuestionPr );
                m_multiplyQuestionListBackup.push_back( ranQuestionPr );
            }
        }
    }
    
    if ( cfg.bIsDivideGrpEnabled && cfg.iDivideQuestionsCnt > 0 ) {
        auto genCnt = 0;
        for(     auto a = cfg.iDivide1RangeMinUIValue;  a <= cfg.iDivide1RangeMaxUIValue; ++a ) {
            for( auto b = cfg.iDivide2RangeMinUIValue;  b <= cfg.iDivide2RangeMaxUIValue; ++b ) {
                if ( b!=0 && a >= b ) {
                    auto ret = a / b;
                    auto mod = a % b;
                    if ( mod == 0 || cfg.bIsResultHaveModEnabled ) {
                        auto bIsValid = true;
                        if ( cfg.bIs9x9  ) {
                            bIsValid = (b>0 && b<=9) && (ret>0 && ret<=9);
                        }

                        if( bIsValid ) {     
                            ++genCnt;
                            m_divideQuestionList.push_back( qMakePair(a,b) );
                            m_divideQuestionListBackup.push_back( qMakePair(a,b) );
                        }
                    } 
                }
            }
        }

        if ( genCnt < cfg.iDivideQuestionsCnt ) {
            auto delta =  genCnt - cfg.iDivideQuestionsCnt;
            for( auto i = 0; i < delta; ++i ) {
                auto ranQuestionPr =  m_divideQuestionList.at( ranMachine->bounded(0, m_divideQuestionList.size() ) );
                m_divideQuestionList.push_back( ranQuestionPr );
                m_divideQuestionListBackup.push_back( ranQuestionPr );
            }
        }
    }

    //
    // set op1 / op2 / result   random
    //
    int table[OP_COUNT][3] = {
        { cfg.iAdd1rate, cfg.iAdd2rate, cfg.iAdd3rate  },
        { cfg.iMinus1rate, cfg.iMinus2rate,  cfg.iMinus3rate  },
        { cfg.iMultiply1rate, cfg.iMultiply2rate,  cfg.iMultiply3rate  },
        { cfg.iDivide1rate, cfg.iDivide2rate,  cfg.iDivide3rate  }
    };

    for( auto op_index = 0; op_index < OP_COUNT; ++op_index ) {
        auto bFlag = false;
        if ( op_index == 0 ) {
            bFlag = ( cfg.bIsAddGrpEnabled && cfg.iAddQuestionsCnt > 0 );
        } else if ( op_index == 1 ) {
            bFlag = ( cfg.bIsMinusGrpEnabled && cfg.iMinusQuestionsCnt > 0 );
        } else if ( op_index == 2 ) {
            bFlag = ( cfg.bIsMultiplyGrpEnabled && cfg.iMultiplyQuestionsCnt > 0 );
        } else {
            bFlag = ( cfg.bIsDivideGrpEnabled && cfg.iDivideQuestionsCnt > 0 );
        }

        if ( bFlag ) {
            for( auto tag = 0; tag < 3; ++tag ) {
                for( auto times = 0; times < table[op_index][tag]; ++times ) {
                    m_3opPossibilities[op_index].push_back( tag );
                    m_3opPossibilitiesBackup[op_index].push_back( tag );
                }
            }
        }
    }

    m_maxQuestionCount = 0;
    if ( cfg.bIsAddGrpEnabled && cfg.iAddQuestionsCnt > 0 ) {
        m_maxQuestionCount += cfg.iAddQuestionsCnt;
        for( auto i = 0; i < cfg.iAddQuestionsCnt; ++ i ) {
            m_4opPossibilities.push_back( 0 );
            m_4opPossibilitiesBackup.push_back( 0 );
        }
    }

    if ( cfg.bIsMinusGrpEnabled && cfg.iMinusQuestionsCnt > 0 ) {
        m_maxQuestionCount += cfg.iMinusQuestionsCnt;
        for( auto i = 0; i < cfg.iMinusQuestionsCnt;  ++ i ) {
            m_4opPossibilities.push_back( 1 );
            m_4opPossibilitiesBackup.push_back( 1 );
        }
    }

    if ( cfg.bIsMultiplyGrpEnabled && cfg.iMultiplyQuestionsCnt > 0 ) {
        m_maxQuestionCount += cfg.iMultiplyQuestionsCnt;
        for( auto i = 0; i < cfg.iMultiplyQuestionsCnt;  ++ i ) {
            m_4opPossibilities.push_back( 2 );
            m_4opPossibilitiesBackup.push_back( 2 );
        }
    }


    if ( cfg.bIsDivideGrpEnabled && cfg.iDivideQuestionsCnt > 0 ) {
        m_maxQuestionCount += cfg.iDivideQuestionsCnt;
        for( auto i = 0; i < cfg.iDivideQuestionsCnt;  ++ i ) {
            m_4opPossibilities.push_back( 3 );
            m_4opPossibilitiesBackup.push_back( 3 );
        }
    }

}

void MainWindow::on_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if ( m_soundTag == 2 && status == QMediaPlayer::EndOfMedia ) {
        qDebug() << "loop Next CD";
        m_pEffectPlayer->setMedia(QUrl( m_cdSoundFileList.at(m_countDownSoundIdx) ));
        m_pEffectPlayer->setVolume(55);
        m_pEffectPlayer->play();

        // loop + 1
        ++m_countDownSoundIdx;
        m_countDownSoundIdx = m_countDownSoundIdx % m_countDownSoundMaxCnt;

        m_soundTag = 2;
    }
}


