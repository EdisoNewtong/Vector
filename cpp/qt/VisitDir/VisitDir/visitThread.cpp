#include "visitThread.h"

#include <QDir>
#include <QFileInfoList>
#include <QDebug>

VisitThread::VisitThread(QObject *parent /* = nullptr */)
	: QThread(parent)
	, m_visitPath("")
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
		return;
	}

	visitDir( qpath.absolutePath(), 0);
	emit onVisitDone(); // !!!Done!!! travelsal the entire dir  end up here
}


void VisitThread::visitDir(const QString &dpath, int level)
{
    // qDebug() << "dpath => " << dpath;
    // ready to visit
    QDir dir2visit(dpath);
    const auto &fileInfoList = dir2visit.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden);
	for ( auto i = 0; i < fileInfoList.size(); ++i ) 
	{
		const auto &finfo = fileInfoList.at(i); 

		auto strpath = finfo.absoluteFilePath();
        // qDebug() << "\tvisit => " << strpath;
		//
		// Useful API:
		//		finfo.isFile();    or    finfo.isDir();
		// 0: dir
		// 1: file
		// 2: others
		if ( finfo.isDir() ) {
			// travelsal folder recursively
			emit onStartVisit(strpath, 0, level+1);
            visitDir( strpath,  level+1);
		} else if ( finfo.isFile() ) {
			// visit file
			emit onStartVisit(strpath, 1, level);
		} else {
			// the path is neither dir nor file , it exists with special type ?
			emit onStartVisit(strpath, 2, level);
		}
	}

}


void VisitThread::setVisitPath(const QString &path)
{
	m_visitPath = path;
}


