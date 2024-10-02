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

private:
    virtual void run() Q_DECL_OVERRIDE;
            void travelsalDirs(const QDir& d, unsigned long long layer);

protected:
    myVisitThread::E_THREAD_TYPE m_type;
    QDir         m_targetDir;
    QList<QDir>* m_pVisitedDirs;
signals:
    void visitOneDir(int idx, const QFileInfo& finfo, unsigned long long layer);
    void visitOneFile(int idx, const QFileInfo& finfo);
    void visitOneDirAllFiles(unsigned long long dirCnt);

    void selectAllDirs();
    void visitAllDone();



};








