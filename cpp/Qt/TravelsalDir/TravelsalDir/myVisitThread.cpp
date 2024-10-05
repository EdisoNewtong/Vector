#include "myVisitThread.h"
#include <QDebug>

static const QChar G_SC_OPTION_SEP(';');
static const QString G_SC_WILDCARD_PREFIX("*.");
static const QChar G_SC_DOT('.');


myVisitThread::myVisitThread(QObject* parent /* = nullptr */)
    : QThread(parent)
    , m_type( E_DIRS )
    , m_targetDir("")
    , m_pVisitedDirs( nullptr )
    , m_bTerminateFlag( false )
	, m_bIsIgnoreDirCaseSensitive( true )
    , m_bPickDirOnly( false )
	, m_strIgnoreDirPatternGrp()
	, m_bIsIgnoreFileCaseSensitive( true )
    , m_bPickFileOnly( false )
	, m_strIgnoreFilePatternGrp()
{
    // QThread::setTerminationEnabled(true);
}


void myVisitThread::setType(myVisitThread::E_THREAD_TYPE type)
{
    m_bTerminateFlag = false;
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
				if ( m_bTerminateFlag ) {
					break;
				}

                auto dInfo = *it;
                auto fileList = dInfo.entryInfoList( QDir::NoDotAndDotDot | QDir::Files | QDir::Hidden );


                for ( auto i = 0; i < fileList.size(); ++i ) {
					//
					// Force stop
					//
					if ( m_bTerminateFlag ) {
						break;
					}

					auto dirInfo = fileList.at(i);
					auto isMatched = false;
					for( auto i = 0; i <  m_strIgnoreFilePatternGrp.size(); ++i )
					{
						auto pattern = m_strIgnoreFilePatternGrp.at(i);
						// start wit  "*."
                        if ( !pattern.isEmpty() ) {
                            if ( pattern.startsWith(G_SC_WILDCARD_PREFIX) ) {
                                auto ext = pattern.mid(2);
                                // such as   *.config.backup
                                QString realSuffix;
                                if ( ext.contains(G_SC_DOT) ) { // the suffix is more than one '.'
                                    realSuffix = dirInfo.completeSuffix();
                                } else {
                                    realSuffix = dirInfo.suffix();
                                }

                                isMatched = (realSuffix.compare(ext, (m_bIsIgnoreFileCaseSensitive ? Qt::CaseInsensitive : Qt::CaseSensitive)) == 0);
                            } else {
                                // use startsWith strategy to compare with file and its pattern
                                isMatched = ( dirInfo.fileName().startsWith(pattern, (m_bIsIgnoreFileCaseSensitive ? Qt::CaseInsensitive : Qt::CaseSensitive)) );
                            }

                            if ( isMatched ) {
                                break;
                            }
                        }

					}

                    auto bNeedVisitFile = false;
                    if ( m_bPickFileOnly ) {
                        bNeedVisitFile = isMatched;
                    } else {
                        bNeedVisitFile = !isMatched;
                    }

					if ( bNeedVisitFile ) {
						emit visitOneFile( (i+1) ,  dirInfo );
					} else {
						// skip this file , maybe TODO something
					}
                }

				if ( m_bTerminateFlag ) {
					break;
				}

                visitOneDirAllFiles( folderIdx+1 );
            }

            emit visitAllDone();
        }
    }
}

void myVisitThread::setVisitedDirs(QList<QDir>* dirs)
{
    m_bTerminateFlag = false;
    m_pVisitedDirs = dirs;
}

void myVisitThread::setStartDir(const QDir& t)
{

    m_bTerminateFlag = false;
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
        //
        // Force stop
        //
        if ( m_bTerminateFlag ) {
            break;
        }

        auto dPath = *it; 

		auto isMatched = false;
		for( auto i = 0; i < m_strIgnoreDirPatternGrp.size(); ++i )
		{
			auto pattern = m_strIgnoreDirPatternGrp.at(i);
			// use strategy to compare with
            if ( !pattern.isEmpty() ) {
                isMatched = ( dPath.fileName().startsWith( pattern, (m_bIsIgnoreDirCaseSensitive ? Qt::CaseInsensitive : Qt::CaseSensitive) ) );
                if ( isMatched ) {
                    break;
                }
            }
		}

        auto bNeedVisit = false;
        if ( m_bPickDirOnly ) {
            bNeedVisit = isMatched;
        } else {
            // skip this dir
            bNeedVisit = !isMatched;
        }

		if ( bNeedVisit ) {
			// qDebug() << "\tLoop : " << dPath; // << " -> layer = " << layer;
			emit visitOneDir(idx, dPath, layer);
		} else {
			// skip this dir , maybe TODO something
		}

        travelsalDirs( QDir( dPath.absoluteFilePath() ), (layer+1) );
    }
}

void myVisitThread::setTerminateFlag()
{
    m_bTerminateFlag = true;
}



void myVisitThread::setDirIgnoreOption(bool bIsIgnoreCaseSensitive, const QString& strIgnorePattern, bool bPickOnly)
{
	m_bIsIgnoreDirCaseSensitive = bIsIgnoreCaseSensitive;
    m_bPickDirOnly = bPickOnly;

	QString pattern = strIgnorePattern.trimmed();
	m_strIgnoreDirPatternGrp.clear();
    if ( !pattern.isEmpty() ) {
        m_strIgnoreDirPatternGrp = pattern.split(G_SC_OPTION_SEP);
        for( auto& p : m_strIgnoreDirPatternGrp ) {
            p = p.trimmed();
        }
    }
}


void myVisitThread::setFileIgnoreOption(bool bIsIgnoreCaseSensitive, const QString& strIgnorePattern, bool bPickOnly)
{
	m_bIsIgnoreFileCaseSensitive = bIsIgnoreCaseSensitive;
    m_bPickFileOnly = bPickOnly;

	QString pattern = strIgnorePattern.trimmed();
	m_strIgnoreFilePatternGrp.clear();
    if ( !pattern.isEmpty() ) {
        m_strIgnoreFilePatternGrp  = pattern.split(G_SC_OPTION_SEP);
        for( auto& p : m_strIgnoreFilePatternGrp ) {
            p = p.trimmed();
        }
    }
}


