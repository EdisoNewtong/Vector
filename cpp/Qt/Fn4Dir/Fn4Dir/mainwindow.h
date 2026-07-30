#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "workthread.h"
#include "fnedcore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_goBtn_clicked();

    void on_encBtn_clicked();

    void on_decBtn_clicked();

    void on_cmpBtn_clicked();



    void on_updateFileListProcessState(const QString& msg);
    void on_updateFileListPrograssBar(float per);
    void on_encDecFinished(const QString& msg);
    void on_updateFileContent(float per);
    void onThreadDone();

private:
    // method
    QThread::Priority getThreadPriority();

private:
    Ui::MainWindow *ui;
    WorkThread *m_pThread;
    FNEDCore   *m_pEncDecObject;
};
#endif // MAINWINDOW_H
