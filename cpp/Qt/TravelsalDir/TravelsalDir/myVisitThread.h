#pragma once

#include <QThread>
#include <QList>
#include <QDir>

class myVisitThread : public QThread
{
    Q_OBJECT
public:
    enum E_THREAD_TYPE {
        E_DIRS,
        E_FILES,
    };

    myVisitThread(QObject* parent = nullptr);


    void setType(myVisitThread::E_THREAD_TYPE type);
    void setVisitedDirs(QList<QDir>* dirs);
    void setStartDir(const QDir& t);
    void setTerminateFlag();

	void setDirIgnoreOption(bool bIsIgnoreCaseSensitive, const QString& strIgnorePattern, bool bPickOnly);
	void setFileIgnoreOption(bool bIsIgnoreCaseSensitive, const QString& strIgnorePattern, bool bPickOnly);
private:
    virtual void run() Q_DECL_OVERRIDE;
            void travelsalDirs(const QDir& d, unsigned long long layer);

protected:
    myVisitThread::E_THREAD_TYPE m_type;
    QDir         m_targetDir;
    QList<QDir>* m_pVisitedDirs;
    bool         m_bTerminateFlag;

	bool             m_bIsIgnoreDirCaseSensitive;
    bool             m_bPickDirOnly; // true: ignore folder ,  false: only pick matched folder
	QStringList      m_strIgnoreDirPatternGrp;

	bool             m_bIsIgnoreFileCaseSensitive;
    bool             m_bPickFileOnly; // true: ignore file(s) ,  false: only pick matched file(s)
	QStringList      m_strIgnoreFilePatternGrp;
signals:
    void visitOneDir(int idx, const QFileInfo& finfo, unsigned long long layer);
    void visitOneFile(int idx, const QFileInfo& finfo);
    void visitOneDirAllFiles(unsigned long long dirCnt);

    void selectAllDirs();
    void visitAllDone();



};








