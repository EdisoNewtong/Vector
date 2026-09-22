#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRadioButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define EOL_MODE_READ_CNT      2
#define EOL_TYPE_CNT      5
#define EOL_CONVERT_CNT   3

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum E_READ_MODE {
        E_LAZY_MODE = 0,
        E_FULL_MODE = 1,
    };

    enum E_EOL_TYPE {
        E_UNIX   =  0,  // \n
        E_WIN    =  1,  // \r\n
        E_MAC    =  2,  // \r
        E_HYBRID =  3,  // ??
        E_NOT_DETECHED =  4,  // no \n \r\n scanned
        E_EOL_CNT = EOL_TYPE_CNT,
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_loadBtn_clicked();
    void on_saveBtn_clicked();
    void on_rescanBtn_clicked();

    void onDropedFile(const QString& fPath, bool bReadSucc, qint64 readed, qint64 totally, const QByteArray& buffer);

    void on_delEOLBtn_clicked();

protected:
    void doBufferAnalyze(const QString& fPath, bool bReadSucc, qint64 readed, qint64 totally, const QByteArray& buffer, bool isDrop);
    void resetUI();
    bool doConvert(const QString& fPath, int originalEol, int newEolType, bool bNeedAddeolAtLast, QString& errorMsg);
    bool doDeleteEndEOL(QString& errorMsg);

    QString genTmpFileName(const QString& srcAbsPath);
private:
    Ui::MainWindow *ui;

    QRadioButton *m_modeBtnAry[EOL_MODE_READ_CNT];
    QRadioButton *m_originalBtnAry[EOL_TYPE_CNT];
    QRadioButton *m_convertBtnAry[EOL_CONVERT_CNT];
    const char*   m_EOL_NAME[EOL_TYPE_CNT];
    int           m_eolType;
    int           m_bReadState;
    bool          m_bLastEOLFlag;
    QString       m_loadedFilePath;
};
#endif // MAINWINDOW_H
