#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QDir>
#include <QMap>
#include <QElapsedTimer>

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
    void on_scanBtn_clicked();

    void on_clearBtn_clicked();

    void on_stopBtn_clicked();

    void on_slashFlipBtn_clicked();

    void on_previousBtn_clicked();

    void on_nextBtn_clicked();
    void on_pickFolderInput_returnPressed();

    void on_refreshDirBtn_clicked();

    void onVisitOneDir(int idx, const QFileInfo& finfo,unsigned long long layer);
    void onVisitOneFile(int idx, const QFileInfo& finfo);

    void onVisitOneDirAllFiles(unsigned long long finishedCnt);

    void onVisitAllDirsFinished();
    void onVisitAllFileFinished();

    void on_actionMultiThread_triggered();

    void on_actionSingleThread_triggered();


private:
    void initUI();
    void refreshFileSystemModel(bool needDeletePrevious);

    void visitDir( const QDir& toBeTravelsaled, unsigned long long layer);
private:
    Ui::MainWindow *ui;
    QFileSystemModel* m_pFileSystemModel;

    QElapsedTimer                          m_timerTotal;
    QElapsedTimer                          m_timerDirs;
    QElapsedTimer                          m_timerFiles;
    bool                                   m_bUseMultiThreadMode;

    unsigned long long m_visitedDirCnt;
    unsigned long long m_visitedFileCnt;
    unsigned long long m_maxLayer;

    QMap<QString, unsigned long long> m_extensionMap;
    QMap<unsigned long long, QList<QDir> > m_depthDirs;
    QList<QDir>*                           m_pAllDirs;


};
#endif // MAINWINDOW_H
