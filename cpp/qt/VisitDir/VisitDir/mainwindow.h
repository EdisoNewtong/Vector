#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include <QMap>
// #include <QVector>
#include <QList>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include <QFileInfo>
#include <QJsonDocument>
// class QFileInfo;

struct suffixFileInfo
{
	suffixFileInfo() {
		singleExtList.clear();
		multiExtMap.clear();
	}

    virtual ~suffixFileInfo() {
        for ( auto it = singleExtList.begin(); it!= singleExtList.end(); ++it ) {
            delete *it;
            *it = nullptr;
        }
        singleExtList.clear();

        for ( auto it = multiExtMap.begin(); it!= multiExtMap.end(); ++it ) {
            auto flist = it.value();
            for ( auto innerIt = flist.begin(); innerIt != flist.end(); ++innerIt ) {
                if ( *innerIt != nullptr ) {
                    delete *innerIt;
                    *innerIt = nullptr;
                }
            }
            flist.clear();
        }
        multiExtMap.clear();
    }
    // virtual void dummyFunc() { }
	

	QList<QFileInfo*>                     singleExtList;
	QMap<QString, QList<QFileInfo*> >     multiExtMap;
};







class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
	void onVisitSomething(const QString &what,int type, int level);
	void onVisitEntireDirDone(int error);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();



private:
	void releaseMemory();
	void processFileBySuffix(const QString &fileAbsPath);

	void loadCacheFileForResult(const QString& dirPath);
	void forceScanDir(const QString& dirPath);
	void saveToRecord();
	void loadFromRecord(const QString& path);
private:
    Ui::MainWindow *ui;
	// QThread*        m_runningThread;
	QString         m_travelsalDirPath;
	QElapsedTimer   m_timer;
	int             m_runningStatus;
	quint64         m_totalFileCnt;

	
	QList<QFileInfo*>               m_noExtFileList;
	QMap<QString, suffixFileInfo*>  m_ExtFileMap;

	QList<QString>				    m_dirList;
};
#endif // MAINWINDOW_H
