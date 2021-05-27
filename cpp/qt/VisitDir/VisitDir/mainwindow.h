#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include <QMap>
#include <QVector>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include <QFileInfo>
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
	

	QVector<QFileInfo*>                     singleExtList;
	QMap<QString, QVector<QFileInfo*> >     multiExtMap;
};







class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
	void onVisitSomething(const QString &what,int type, int level);
	void onVisitEntireDirDone();

    void on_pushButton_clicked();

private:
	void releaseMemory();
	void processFileBySuffix(const QString &fileAbsPath);
private:
    Ui::MainWindow *ui;
	// QThread*        m_runningThread;
	QString         m_travelsalDirPath;
	QElapsedTimer   m_timer;
	int             m_runningStatus;

	
	QVector<QFileInfo*>             m_noExtFileList;
	QMap<QString, suffixFileInfo*>  m_ExtFileMap;
};
#endif // MAINWINDOW_H
