#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QMovie>
#include <QMap>
#include <QMediaPlayer>

#include "settingdialog.h"

#define OP_COUNT   4
#define RELOP_CNT  8

#define MAX_NUMBER 9
#define POSSIBILITIES 100

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum E_START_STATUS
    {
        E_STOPPED,
        E_PAUSED,
        E_STARTED
    };

    enum E_OP_TYPE
    {
        E_PLUS = 0,
        E_MINUS,
        E_MULTIPLY,
        E_DIVIDE,

        E_OP_MAX_CNT,
    };

    enum E_HEADER_TAG
    {
        E_QUESTION_TAG = 0,
        E_COUNT_DOWN_TAG,
        E_YES_NO_UNDECIDE_TAG,
        E_ANSWER_TAG,
        E_CORRECT_RATE_TAG,
        E_INCORRECT_RATE_TAG,

        E_TAG_MAX
    };
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void nextQuestion(bool currentCorrect);
    void initUI();
    void updateCurrentResultUI(bool correct);
    void resetQuestionListByCfg(const Dialog::settingInfo& cfg);

private slots:
    void on_btnYES_clicked();

    void on_btnNO_clicked();

    void on_action_start_triggered();

    void on_action_pause_triggered();

    void on_action_stop_triggered();

    void on_action_settings_triggered();

    void on_action_save_triggered();

    void on_action_load_triggered();


    void on_updateLCDNumber();
    void on_randomQuestion();
    void on_321goFinished();

    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);
private:
    Ui::MainWindow *ui;

    E_START_STATUS m_runningState;

    QTimer          m_questionDuringTick;
    QElapsedTimer   m_elpTimer;

    QTimer          m_countDownTimer;   // 3-2-1 : Go
    int             m_cdTickInterval;

    const int       m_tickInterval;
    qint64          m_tickCount;

/*const*/ int       m_maxQuestionCount;
          int       m_questionIdx;
          int       m_correctCnt;
          int       m_incorrectCnt;

    QMovie*         m_321goGif;
    int             m_countDownSoundIdx;
    const int       m_countDownSoundMaxCnt;
    QList<QString>  m_cdSoundFileList;
    int             m_soundTag;
    int             m_originalFontSize;

    QList<QPair<int,int> >    m_plusQuestionList;
    QList<QPair<int,int> >    m_plusQuestionListBackup;
    QList<QPair<int,int> >    m_minusQuestionList;
    QList<QPair<int,int> >    m_minusQuestionListBackup;
    QList<QPair<int,int> >    m_multiplyQuestionList;
    QList<QPair<int,int> >    m_multiplyQuestionListBackup;
    QList<QPair<int,int> >    m_divideQuestionList;
    QList<QPair<int,int> >    m_divideQuestionListBackup;

    const QString             ms_opRandomPool[OP_COUNT];
    const QString             ms_relopRandomPool[RELOP_CNT];

    QList<int>                m_3opPossibilities[OP_COUNT];
    QList<int>                m_3opPossibilitiesBackup[OP_COUNT];

    QList<int>                m_4opPossibilities;
    QList<int>                m_4opPossibilitiesBackup;

    QMap<int,QString>         m_tagStringMap;
    QString                   m_currentAnswer;
    QString                   m_currentModAnswer;

    QMediaPlayer*             m_pSoundPlayer;
    QMediaPlayer*             m_pEffectPlayer;

    Dialog*                   m_pSettingDlg;

};
#endif // MAINWINDOW_H
