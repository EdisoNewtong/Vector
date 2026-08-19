#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QIcon>
#include <QDir>
#include <QMap>
#include <QElapsedTimer>
#include <QTreeWidgetItem>
#include <QVector>

#include "myVisitThread.h"

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


    void on_resultFileSearhingInput_returnPressed();

    void on_displayFileContent();

    void on_actionSkipDir_triggered();

    void on_actionSkipFile_triggered();

private:
    void initUI();
    void refreshFileSystemModel(bool needDeletePrevious);

    void visitDir( const QDir& toBeTravelsaled, unsigned long long layer);
    void fill_ScanResultIntoTreeView();

    void focusSearchMatched();
    void focusNextBtnMatched();
    void focusPreviousBtnMatched();

    void fill_fileGroupTreeNode();
    void fill_dirGroupTreeNode();

    // Util function(s)
    static QString getHumanReadableSize(qint64 fileSz);
private:
    Ui::MainWindow *ui;
    QFileSystemModel* m_pFileSystemModel;

    bool                                   m_bPickDirs;
    bool                                   m_bPickFiles;

    QString                                m_ignoredFolderPattern;
	QStringList                            m_ignoredFolderPatternList;
    bool                                   m_bIgnoredFolderCaseSensitive;
    bool                                   m_bOnlyVisitMatchedFolder;

    QString                                m_ignoredFilePattern;
	QStringList                            m_ignoredFilePatternList;
    bool                                   m_bIgnoredFileCaseSensitive;
    bool                                   m_bOnlyVisitMatchedExtFiles;

    bool                                   m_bSeperateMultiExtensionFlag;


    QElapsedTimer                          m_timerTotal;
    QElapsedTimer                          m_timerDirs;
    QElapsedTimer                          m_timerFiles;
    bool                                   m_bUseMultiThreadMode;
    bool                                   m_bSkipSymbol_link_dirFlag;
    bool                                   m_bSkipSymbol_link_fileFlag;

    unsigned long long m_visitedDirCnt;
    unsigned long long m_visitedFileCnt;
    unsigned long long m_maxLayer;


    int                m_multiThreadState; // 0:stoped    1:visitDir    2:visitFiles
    myVisitThread*     m_pDirVisitThread;
    myVisitThread*     m_pFileVisitThread;

    QMap<QString, QMap<QString, QVector<QFileInfo>> > m_extensionMap;


    QMap<unsigned long long, QVector<QDir> > m_depthDirs;
    QVector<QDir>*                           m_pAllDirs;

    QVector<QTreeWidgetItem*>                m_generatedTreeNodeList;
    QVector<QTreeWidgetItem*>                m_searchMatchedResultNodeList;
    int                                    m_currentPreviousNextIdx;

    int                                      m_extTypeGroupCnt;
    int                                      m_extTypeGroupPartCnt;


    // Resource
      // --------------------  Icons --------------------
    QIcon*                                  m_treeFileIcon;
    QIcon*                                  m_treeExtIcon;
    QIcon*                                  m_treeDirIcon;
    QIcon*                                  m_treeMtExtIcon;

};
#endif // MAINWINDOW_H
