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
    void fillFileList(const QString& fileListByLine, int subType = 0);

    // Enc
    void setPrintFilesOnlyFlag(bool b);
    void setBuildDirOnly(bool b);
    void setTake_EncryptActionFlag(bool b);

    // Dec
    void setDecOnSameFileFlag(bool  b);
    void setDecAndRenameFileFlag(bool  b);

    void singleThreadRun();

protected:
    virtual void run() Q_DECL_OVERRIDE;

    QString m_oriDir;
    QString m_destDir;
    QString m_targetFiles;

    /****************************************************************************************************
     
      m_cmpSubType :
          0: for enc/dec    
          1: compare 2 files on 2 lines      
          2: compare a lot pairs of files , each pair on one line and be seperated by ';' 

    ****************************************************************************************************/
    int     m_cmpSubType; 
    
    void visitDir();
    void visitDirSample();
    void preparePairsGroup(QVector< QPair<QString,QString> >& refPairGroup);

    void do_EncOp();
    void process_EncTravesalFileList();
    QString getPrint_EncFileList();

    void do_DecOp();
    void process_DecTravesalFileList();
    void process_DecFileList();

    void do_CmpOp();

    void buildNewDirsOnDestintion();

    void travelsalDirWithLayer(const QString& path, int layer);
    void travelsalDirWithLayerSample(const QString& path, int layer);
    void initVaribles();

    QMap<int, QSet<QString> > m_dirs;
    QMap<int, QSet<QString> > m_files;
    QSet<QString> m_createdDirs;
    QStringList               m_sampleFileList;

    int m_randomDirVisitProcessNumber;
    int m_reached_max_layer;
    int m_fileCnt;

    QElapsedTimer m_timer_i;
    QElapsedTimer m_timer_total;

    FNEDCore* m_pEncDecObj;
    E_OP_TYPE m_op;

    bool m_bIsTargetFileReady; // Common for ( Enc && Dec )
    
    // enc options
    bool m_bPrintFileListOnly;
    bool m_bBuildSameDirStructOnly;
    bool m_bTakeEncActionFlag;

    // dec options
    bool m_bDecOnSameFileFlag;
    bool m_bDecAndRenameFileFlag;
signals:
    void updateFileListProcessState(const QString& msg);
    void updateFileListPrograssBar(float percent);

    void encdecFilesFinished(const QString& msg);
    void aNewFileComes(const QString& fileAbsPath);

    void printDirTree(const QString& fileList);

};


