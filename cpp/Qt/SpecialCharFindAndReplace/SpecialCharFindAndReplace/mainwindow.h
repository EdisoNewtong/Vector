#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct WrapQString {
    int startIdx; // start with 0
    int len; // length is either 1 or 2
    WrapQString(int s, int l) : startIdx(s), len(l) { }
};

struct find_replace_object {
    QChar seperator;
    QString original_ch_Parttern;
    QString original_ch_replace_Parttern;
    QList<QString> variants_ch_Parttern;
    bool           enabled;

    find_replace_object()
        : seperator(',') // default seperator
        , original_ch_Parttern("")
        , original_ch_replace_Parttern("")
        , variants_ch_Parttern()
        , enabled( true )
    {
    }
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum E_HEADER_TAG {
        E_ENABLE_STATE = 0, // use Check Box
        E_TAG_NAME,
        E_ORIGINAL_CHAR,
        E_VARIANT_CHAR_LIST,
        E_SEP_CHAR,
        E_DETAIL_INFO,
    };


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private slots:
    void on_loadFileBtn_clicked();

    void on_saveFileBtn_clicked();

    void on_searchBtn_clicked();

    void on_replace1Btn_clicked();

    void on_replaceAllBtn_clicked();

    void on_newCfgBtn_clicked();

    void on_newRowBtn_clicked();

    void on_loadCfgBtn_clicked();

    void on_saveCfgBtn_clicked();

    void on_checkLogicBtn_clicked();

    void on_actionShow_InfoBox_toggled(bool arg1);

    void on_sPrevBtn_clicked();

    void on_sNextBtn_clicked();

    void on_runCmdBtn_clicked();

    void on_actionShow_Command_UserManual_triggered();

    void on_deHighlightBtn_clicked();

    // void on_searchAllBtn_clicked();

    void on_targetScanTextBox_textChanged();

    void on_actionPreviousFind_triggered();

    void on_actionNextFind_triggered();

private:
    void initCfgTable();
    void initUI();
    void splitQString(const QString& text);

    void disableHightLight();
    void highLightAllMatched();
    void highLightOneMatched(bool bShouldClearPreviousHighLight);

    void searchMatched(int startIdx, bool isOneMatchedStop);
private:
    Ui::MainWindow *ui;

    QList<find_replace_object> m_cfgObjectList;
    QList<WrapQString>         m_splitQStringList;
    // 1st element : startIdx
    // 2nd element :   length
    QList< QPair<int,int> >    m_matchedPairList;
    int                        m_TextChangedStateAfterScan; // 0:haven't scan yet    1:scanned     -1:after being scanned  , the text has been changed
    int                        m_matchedIdx;

    //  First  is symbol 
    //  Second is the its meaning
    static const QMap<QString, QString> sc_punctuationMap;
};
#endif // MAINWINDOW_H
