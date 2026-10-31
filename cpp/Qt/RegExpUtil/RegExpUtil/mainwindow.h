#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QRadioButton>
#include <QPair>

#include <QRegularExpression>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

static const int SC_MatchOptCnt   = 3;
static const int SC_MatchTypeCnt  = 4;
static const int SC_PatternOptCnt = 10;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_matchBtn_clicked();

    void on_globalMatchBtn_clicked();

    void on_anchoredBtn_clicked();

    void on_escapleBtn_clicked();

    void on_wildcardReBtn_clicked();

    void on_cppBtn1_clicked();

    void on_checkPatternBtn_clicked();

    void on_matchValidBtn_clicked();
protected:
    void init_UI();
    void init_WidgetAry();

    void test_Study_perl_reg();
    void test_Study_case0();
    void test_Study_case1();

private:
    Ui::MainWindow *ui;

protected:
    QPair<QCheckBox*,unsigned int>      m_uiMatchOptWidgetAry[SC_MatchOptCnt];
    QPair<QRadioButton*,unsigned int>   m_uiMatchTypeWidgetAry[SC_MatchTypeCnt];
    QPair<QCheckBox*,unsigned int>          m_uiPatternOptWidgetAry[SC_PatternOptCnt];

    QRegularExpression::MatchType        getMatchType();
    QRegularExpression::MatchOptions     getMatchOptions();
    QRegularExpression::PatternOptions    getPatternOptions();

};
#endif // MAINWINDOW_H


