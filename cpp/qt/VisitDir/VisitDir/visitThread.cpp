#include "visitThread.h"

#include <QDir>
#include <QFileInfoList>
#include <QDebug>

VisitThread::VisitThread(QObject *parent /* = nullptr */)
	: QThread(parent)
	, m_visitPath("")
	, m_includeHiddenFlag(false)
{
}

// virtual 
void VisitThread::run() // override
{
	/*
	 * Do Real Visit   by the given path stored in m_visitPath
	*/
    QDir qpath(m_visitPath);
	if ( !qpath.exists() ) 
	{
		// emit resultReady(result);
		emit onVisitDone(0);
		return;
	}

	visitDir( qpath.absolutePath(), 0);
	emit onVisitDone(1); // !!!Done!!! travelsal the entire dir  end up here
}


void VisitThread::visitDir(const QString &dpath,  int level)
{
    // qDebug() << "dpath => " << dpath << " , level = " << level;
    // ready to visit
    QDir dir2visit(dpath);
	QDir::Filters filterFlag;
	if ( m_includeHiddenFlag ) {
		filterFlag = QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden;
	} else {
		filterFlag = QDir::NoDotAndDotDot | QDir::AllEntries;
	}
    const auto &fileInfoList = dir2visit.entryInfoList( filterFlag );
	
	if( level == 0 ) {
		emit onGetLevelItemCount(fileInfoList.size() );
	}

	for ( auto i = 0; i < fileInfoList.size(); ++i ) 
	{
		const auto &finfo = fileInfoList.at(i); 

		auto strpath = finfo.absoluteFilePath();
		// 0: dir
		// 1: file
		// 2: others
		if ( finfo.isDir() ) {
			// travelsal folder recursively
			emit onStartVisit(strpath, 0, level+1);
            visitDir( strpath, level+1);
		} else if ( finfo.isFile() ) {
			// visit file
            emit onStartVisit(strpath, 1, level);
		} else {
			// the path is neither dir nor file , it exists with special type ?
			emit onStartVisit(strpath, 2, level);
		}

		if( level == 0 ) {
			emit onFinishScanLevelItem(i);
		}
	}

}


void VisitThread::setVisitPath(const QString &path)
{
	m_visitPath = path;
}


void VisitThread::setIncludeFlag(bool flag)
{
	m_includeHiddenFlag = flag;
}

