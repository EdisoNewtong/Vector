#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
// #include <QImage>
#include <QByteArray>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_clearContentBtn_clicked();

    void on_openFileBtn_clicked();

    void on_saveBtn_clicked();

    void on_moveBtn_clicked();

    void on_selectBtn_clicked();

    void on_insertBtn_clicked();

    void on_summaryBtn_clicked();

    void on_checkBox_stateChanged(int arg1);

    void onBinEditorTextCursorChanged();
    void onBinEditorTextContentChanged();
    void onInvalidKeyInput(const QString& hints,bool errorFlag);

    void on_cleanErrMsgBtn_clicked();


protected:
    void reportOperationResult(const QString& msg, bool isError);
    bool travelsalBinaryEditor(QString& errMsg);
    bool travelsalBinaryEditor_Unused(QString& errMsg);
    bool checkCharByteArrayValid(const QByteArray& ba);
    bool isQCharInRange(const QChar& ch, bool needForceCapitalLetter);
    bool checkHexFormatValid(const QString& in, QString& out);
    int  checkInsertPositionIsValid();
    QString getPrintOutBuffer();

    static const int sc_ArySize = 256;
private:
    Ui::MainWindow *ui;
    QByteArray  m_BufferArray;
    QMap<int,QString> m_charMapString;
    bool              m_BufferChangedFlag;

    // QImage m_ByteImageAry[sc_ArySize];
};

#endif // MAINWINDOW_H
