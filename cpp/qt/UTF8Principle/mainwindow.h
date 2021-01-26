#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

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
    void on_qstringRadBtn_clicked();

    void on_unicodeRadBtn_clicked();

    void on_uniBtn_clicked();

    void on_utf8Btn_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void onSelectFontChanged(int idx);
    void updateFont();
protected:
    void initUI();
    void updateGrp();
    void updateSub();

    QString calcQString(const QString& content);
    QString calcU32(const QString& content);
    QString calcUtf8(const QString& u1, const QString& u2, const QString& u3, const QString& u4);
    QString auxOutput(const QString& singleCh, unsigned int u32code);
private:
    Ui::MainWindow *ui;

    bool m_stringMode; // true:QString Mode / false:Unicode or UTF-8 Mode
    bool m_unicodeMode; // true:Unicode / false:UTF-8 Mode
    bool m_timerStart;
    int  m_curFontIdx;

    QTimer m_fontTimer;
};

#endif // MAINWINDOW_H
