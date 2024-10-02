#include "myVisitThread.h"
#include <QDebug>

myVisitThread::myVisitThread(QObject* parent /* = nullptr */)
    : QThread(parent)
    , m_type( E_DIRS )
    , m_targetDir("")
    , m_pVisitedDirs( nullptr )
{

}


void myVisitThread::setType(myVisitThread::E_THREAD_TYPE type)
{
    m_type = type;
}



// virtual
void myVisitThread::run() // Q_DECL_OVERRIDE;
{
    if ( m_type == myVisitThread::E_DIRS ) {
        travelsalDirs(m_targetDir, 0ULL);

        emit selectAllDirs();
    } else {

        // m_type == myVisitThread::E_FILES
        if ( m_pVisitedDirs!=nullptr ) {

            unsigned long long folderIdx = 0;
            for( auto it = m_pVisitedDirs->begin(); it != m_pVisitedDirs->end(); ++it, ++folderIdx ) {
                auto dInfo = *it;
                auto fileList = dInfo.entryInfoList( QDir::NoDotAndDotDot | QDir::Files | QDir::Hidden );
                for ( auto i = 0; i < fileList.size(); ++i ) {
                    emit visitOneFile( (i+1) ,  fileList.at(i) );
                }

                visitOneDirAllFiles( folderIdx+1 );
            }

            emit visitAllDone();
        }
    }
}

void myVisitThread::setVisitedDirs(QList<QDir>* dirs)
{
    m_pVisitedDirs = dirs;
}

void myVisitThread::setStartDir(const QDir& t)
{
    m_targetDir = t;
    // emit visitOneDir(0, QFileInfo( t.path() ), 0);
}

void myVisitThread::travelsalDirs(const QDir& d, unsigned long long layer)
{
    // qDebug() << "\tBeg : " << d << " . " << layer;
    auto dirList = d.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden );
    int idx = 0;
    // qDebug() << "\tdirList : " << dirList;
    for( auto it = dirList.begin(); it != dirList.end(); ++it, ++idx ) {
        auto dPath = *it; 

        // qDebug() << "\tLoop : " << dPath; // << " -> layer = " << layer;
        emit visitOneDir(idx, dPath, layer);

        travelsalDirs( QDir( dPath.absoluteFilePath() ), (layer+1) );
    }
}


