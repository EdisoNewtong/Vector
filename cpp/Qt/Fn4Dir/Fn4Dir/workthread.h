#pragma once

#include <QObject>
#include <QThread>
#include <QMap>
#include <QSet>
#include <QElapsedTimer>

#include "fnedcore.h"


class WorkThread : public QThread
{
Q_OBJECT
public:
    enum E_OP_TYPE
    {
        E_NOP = 0,
        E_ENC_OP, 
        E_DEC_OP,
        E_CMP_OP,
    };

public:
    WorkThread(QObject* parent = nullptr);
    virtual ~WorkThread(); // Q_DECL_OVERRIDE;

    void setOp(E_OP_TYPE op);
    void setPath(const QString& original,const QString& dest, FNEDCore* pEncDecObj);

    void setPrintFilesOnlyFlag(bool b);
    void setBuildDirOnly(bool b);

    void singleThreadRun();

protected:
    virtual void run() Q_DECL_OVERRIDE;

    QString m_oriDir;
    QString m_destDir;

    void visitDir();

    void processEncFileList();
    void processDecFileList();
    void processCmpFileList();

    void buildNewDirsOnDestintion();

    void travelsalDirWithLayer(const QString& path, int layer);
    void initVaribles();

    QMap<int, QSet<QString> > m_dirs;
    QMap<int, QSet<QString> > m_files;
    QSet<QString> m_createdDirs;

    int m_randomDirVisitProcessNumber;
    int m_reached_max_layer;
    int m_fileCnt;

    QElapsedTimer m_timer_i;

    FNEDCore* m_pEncDecObj;
    E_OP_TYPE m_op;

    bool m_bPrintFileListOnly;
    bool m_bBuildSameDirStructOnly;

signals:
    void updateFileListProcessState(const QString& msg);
    void updateFileListPrograssBar(float percent);

    void encdecFilesFinished(const QString& msg);
    void aNewFileComes(const QString& fileAbsPath);

};


