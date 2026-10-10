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

    void on_mtThreadBtn_stateChanged(int checkState);

    void on_decWithNewNameBtn_stateChanged(int checkState);

    void on_outputFileBtn_clicked();

    // void on_givenFileListBtn_clicked();

    void on_buildDirOnlyBtn_clicked();

    // void on_cmpSplitBtn_clicked();

    void on_decWithNewNameBtn_clicked();

    void on_decWithRenameBtn_clicked();

    void on_givenFileListBtn_stateChanged(int checkState);

    void on_cmpSplitBtn_stateChanged(int checkState);

    void on_printFileList(const QString& fileListSortByLayers);
    void on_updateProcessingFileName(const QString& fileName);
private:
    // method
    QThread::Priority getThreadPriority();

private:
    Ui::MainWindow *ui;
    WorkThread *m_pThread;
    FNEDCore   *m_pEncDecObject;
};
#endif // MAINWINDOW_H



