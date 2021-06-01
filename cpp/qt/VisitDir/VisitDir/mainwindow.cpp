#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "visitThread.h"

#include <QDir>
#include <QFileInfoList>
#include <QDebug>
#include <QScrollBar>
#include <QFileInfo>
#include <QTime>
#include <QChar>


static constexpr bool G_SHOW_DETAIL = false;

static const QString G_BASE_GEN_DIR("Travelsal_Log");
static const QString G_ROOT_STR("RootDir = ");
static const QString G_NO_EXT_FILE_FLAG("<No Extension File>");
static const QString G_TAB_1("\t");
static const QString G_TAB_2("\t");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, m_travelsalDirPath("")
	// , m_runningThread(nullptr)
	, m_runningStatus(0)
	, m_totalFileCnt(0ULL)
{
    ui->setupUi(this);
	setWindowTitle( QStringLiteral("Travelsal Dir Recursively") );
    // m_extensionFileMap.clear();

	releaseMemory();
}

MainWindow::~MainWindow()
{
	releaseMemory();
    delete ui;
}


void MainWindow::releaseMemory()
{
	m_dirList.clear();

	for( auto it = m_noExtFileList.begin(); it != m_noExtFileList.end(); ++it ) {
		if ( *it != nullptr ) {
			delete *it;
			*it = nullptr;
		}
	}
	m_noExtFileList.clear();

	for( auto it = m_ExtFileMap.begin(); it != m_ExtFileMap.end(); ++it ) {
		if ( it.value() != nullptr ) {
			delete it.value();
			it.value() = nullptr;
		}
	}
	m_ExtFileMap.clear();
}

/*

void testOnly()
{

 	QString path2Visit = ui->visitPath->toPlainText();
	qDebug() << "path2Visit = " << path2Visit;
	QDir tst(path2Visit);
	const auto& lst = tst.entryInfoList();
	qDebug() << "lst.size() = " << lst.size();
	for ( int i = 0; i < lst.size(); ++i ) {
		auto info = lst.at(i);
	   qDebug() << (i+1) << ". " << endl
				 << "\t" << ".absoluteDir()       = " << info.path() << endl
				 << "\t" << ".absoluteFilePath()  = " << info.absoluteFilePath() << endl
				 << "\t" << ".absolutePath()      = " << info.absolutePath() << endl
				 << "\t" << ".baseName()          = " << info.baseName() << endl
				 << "\t" << ".canonicalFilePath() = " << info.canonicalFilePath() << endl
				 << "\t" << ".canonicalPath()     = " << info.canonicalPath() << endl
				 << "\t" << ".completeBaseName()  = " << info.completeBaseName() << endl
				 << "\t" << ".suffix()            = " << info.suffix() << endl
				 << "\t" << ".completeSuffix()    = " << info.completeSuffix() << endl
				 << "\t" << ".fileName()          = " << info.fileName() << endl
				 << "\t" << ".filePath()          = " << info.filePath() << endl
				 << "\t" << ".path()              = " << info.path() << endl;
	}

}

*/


void MainWindow::loadCacheFileForResult(const QString& dirPath)
{
	releaseMemory();
	loadFromRecord(dirPath);
}

void MainWindow::forceScanDir(const QString& path2Visit)
{
	//
	// Already running
	//
	if ( m_runningStatus == 1 ) {
		return;
	} 

	// otherwise  m_runningStatus != 1  , it must be == 0
	releaseMemory();
	// m_extensionFileMap.clear();
    ui->statusbar->showMessage("Running ... ");

	m_travelsalDirPath = path2Visit;
	ui->outputBox->setPlainText(QString(R"(Begin Travelsal Dir  => %1
)").arg(path2Visit) );
	m_runningStatus = 1;
	m_totalFileCnt = 0;

	auto workthread = new VisitThread();
	workthread->setVisitPath(path2Visit);
	// void onStartVisit(const QString &dPath,int type, int level);
	connect(workthread, &VisitThread::onStartVisit, this, &MainWindow::onVisitSomething );
	connect(workthread, &VisitThread::onVisitDone, this,  &MainWindow::onVisitEntireDirDone );
    connect(workthread, &VisitThread::finished, workthread, &QObject::deleteLater );
	workthread->start();
	qDebug() << "Start Time : " << QTime::currentTime();
	m_timer.restart();
}


void test1()
{
}

void MainWindow::on_pushButton_clicked()
{	
	// ::test1();
	// return;

	QString dirPath = ui->visitPath->toPlainText();
	dirPath = dirPath.trimmed();
	if ( dirPath.isEmpty() ) {
		ui->outputBox->setPlainText(R"(
==================================================
[Error] : input Dir is invaild
==================================================
)");
		return;
	}

	auto useCacheFlag = ( ui->checkBox->checkState() == Qt::Checked );
	if ( !useCacheFlag  ) {
		forceScanDir(dirPath);
	} else {
		loadCacheFileForResult(dirPath);
	}
}


void MainWindow::onVisitSomething(const QString &what, int type, int level)
{
    Q_UNUSED(level)

	constexpr auto printType = 1;
	if ( printType == 1 ) {
		// auto oldstr = ui->outputBox->toPlainText();
		// QString visitTpName;

		if ( type == 0 ) {
			// visitTpName = "Dir";
			m_dirList.push_back(what);
		} else if ( type == 1 ) {
			// m_noneExtensionFileList
			++m_totalFileCnt;
			processFileBySuffix(what);
			// visitTpName = "File";
		} else {
			qDebug() << "[EdisonLog] : Strange File Type ?? ";
			// visitTpName = "Unknown-File";
		}

		// auto newstr = oldstr + "\n" + QString("%1-<%2> : %3").arg(level).arg(visitTpName).arg(what);
		// ui->outputBox->setPlainText(newstr);
	} else {
		
	}
}



void MainWindow::onVisitEntireDirDone(int error)
{
	auto oldstr = ui->outputBox->toPlainText();

	auto duringms = m_timer.elapsed();
	qDebug() << "End   Time : " << QTime::currentTime();


	auto intSecs = duringms / 1000L;
	QString strUsedTime;
	int hours = static_cast<int>(intSecs / 3600);
	int mins = (intSecs / 60) % 60;
	int secs = intSecs % 60;

	QString strFloatPart; 
	strFloatPart.setNum( (duringms / 1000.0f) - intSecs );
	// qDebug() << "strFloatPart = " << strFloatPart;
	int idxOfDot = strFloatPart.indexOf(".");
	if ( idxOfDot != -1 ) {
		auto restPos = idxOfDot + 1;
		if(  restPos < strFloatPart.length() ) {
			strFloatPart = strFloatPart.mid(restPos);
			if ( strFloatPart.length() >= 2 ) {
				strFloatPart = strFloatPart.mid(0,2);
			}
		} else {
			strFloatPart = "00";
		}
	} else {
		strFloatPart = "00";
	}
	
	if ( hours > 0 ) {
		strUsedTime += (QString("%1 hours : ").arg(hours));
		strUsedTime += (QString("%1 mins : ").arg(mins));
		strUsedTime += (QString("%1.%2 secs ").arg(mins).arg(strFloatPart));
	} else {
		if ( mins > 0 ) {
			strUsedTime += (QString("%1 mins : ").arg(mins));
			strUsedTime += (QString("%1.%2 secs ").arg(secs).arg(strFloatPart));
		} else {
			strUsedTime += (QString("%1.%2 secs ").arg(secs).arg(strFloatPart));
		}
	}



	auto newstr = oldstr + "\n" + QString(R"(End Travelsal Dir  => %1 , Used  %2 seconds
)").arg(m_travelsalDirPath).arg(strUsedTime);

	// ui->outputBox->setPlainText(newstr);

    ui->statusbar->showMessage( QString("Done!!     Elapsed Time  =>    %1 ").arg(strUsedTime) );

	if ( error == 0 ) {
		newstr += "=======================================================\n";
		newstr += QString("Error Travelsal : %1    is not a dir \n").arg( m_travelsalDirPath );
		newstr += "=======================================================\n";
		ui->outputBox->setPlainText(newstr);
		return;
	}

	newstr += QString("Totally Dir's  Count =  %1\n").arg( m_dirList.size() ) ;
	newstr += QString("Totally File's Count =  %1\n\n").arg(m_totalFileCnt );

	int idx = 1;
	newstr += QString("None Extension File List.size() = %1\n").arg( m_noExtFileList.size() );
	if ( G_SHOW_DETAIL ) {
		if ( !m_noExtFileList.empty() ) {
			newstr += QString("==================================================\n");
		}
		for ( auto it = m_noExtFileList.begin(); it != m_noExtFileList.end(); ++it, ++idx ) {
			QFileInfo* pFileInfo = *it;
			if ( pFileInfo != nullptr ) {
				newstr += QString("\t%1. %2\n").arg(idx).arg( pFileInfo->fileName() );
			} else {
				newstr += QString("\t%1. pFileInfo == nullptr\n").arg(idx);
			}
		}
		if ( !m_noExtFileList.empty() ) {
			newstr += QString("==================================================\n");
		}
	}

	idx = 1;
	newstr += QString("Extension File Type's size = %1\n").arg( m_ExtFileMap.size() );
	for ( auto it = m_ExtFileMap.begin(); it != m_ExtFileMap.end(); ++it, ++idx ) {
		auto singleCnt = 0;
		auto multiCnt  = 0;
		suffixFileInfo* pSuffixInfo = it.value();
		if ( pSuffixInfo != nullptr ) {
			// calc single extention
			singleCnt += static_cast<int>( pSuffixInfo->singleExtList.size() );

			for (  auto innerIt = pSuffixInfo->multiExtMap.begin(); innerIt != pSuffixInfo->multiExtMap.end(); ++innerIt ) {
				// calc multi extention
				multiCnt += static_cast<int>( innerIt.value().size() );
			}
			newstr += QString("%1    \".%2\"   size = %3, single = %4, multi = %5\n").arg(idx).arg( it.key() ).arg( singleCnt + multiCnt).arg(singleCnt).arg( multiCnt);

			auto idxCnt = 1;
			if ( G_SHOW_DETAIL ) {
				//
				// Single Part
				//
				if ( !pSuffixInfo->singleExtList.empty() ) {
					newstr += QString("      ===== Single List Begin : %1 ===== \n").arg( singleCnt );
				}
				for ( auto innerIt = pSuffixInfo->singleExtList.begin(); innerIt != pSuffixInfo->singleExtList.end(); ++innerIt, ++idxCnt ) {
					QFileInfo* pFInfo = *innerIt;
					if ( pFInfo != nullptr ) {
						newstr += QString("        %1. %2 \n").arg( idxCnt ).arg( pFInfo->fileName() );
					} else {
						newstr += QString("        %1. pFInfo = nullptr \n").arg( idxCnt );
					}
				}
				if ( !pSuffixInfo->singleExtList.empty() ) {
					newstr += QString("      ===== Single List End : %1 ===== \n").arg( singleCnt );
				}
			} else {
				newstr += QString("          \".%1\"  count = %2\n").arg( it.key() ).arg( singleCnt );
			}

			//
			// Multi Part
			//
			auto multiTypeIdx = 1;
			for ( auto innerIt = pSuffixInfo->multiExtMap.begin(); innerIt != pSuffixInfo->multiExtMap.end(); ++innerIt, ++multiTypeIdx  ) {
				auto  multiExt = innerIt.key();
				auto& lst = innerIt.value();

				if ( G_SHOW_DETAIL ) {
					if ( !lst.empty() ) {
						newstr += QString("    \".%1\" ===== Multi-Ext List Begin : %2 ===== \n").arg( multiExt ).arg( lst.size() );
					}

					// travelsal all file
					idxCnt = 1;
					for ( auto fIt = lst.begin(); fIt != lst.end(); ++fIt, ++idxCnt ) {
						QFileInfo* pInfo = *fIt;
						if ( pInfo != nullptr ) {
							newstr += QString("      %1. %2 \n").arg( idxCnt ).arg( pInfo->fileName() );
						} else {
							newstr += QString("      %1. nullptr\n").arg( idxCnt );
						}
					}

					if ( !lst.empty() ) {
						newstr += QString("    \".%1\" ===== Multi-Ext List End : %2 ===== \n").arg( multiExt ).arg( lst.size() );
					}
				} else {
					newstr += QString("          \".%1\" .count = %2\n").arg( multiExt ).arg( lst.size() );
				}
			}

		}
	}

	ui->outputBox->setPlainText(newstr);

	qDebug() << "\tWriteRecord Begin   Time : " << QTime::currentTime();
	saveToRecord();
	qDebug() << "\tWriteRecord End     Time : " << QTime::currentTime();

	m_runningStatus = 0;
}

void MainWindow::processFileBySuffix(const QString &fileAbsPath)
{
	QFileInfo *pFInfo = new QFileInfo(fileAbsPath);
	auto shortSuffix  = pFInfo->suffix();
	auto entireSuffix = pFInfo->completeSuffix();
	// qDebug() << "abs = " << fileAbsPath << endl
	// 	     << "\t suffix = " << entireSuffix; 
	if ( entireSuffix.isEmpty() ) {
		m_noExtFileList.push_back(pFInfo);
	} else {
		// Has Ext
		auto foundIt = m_ExtFileMap.find(shortSuffix);
		if ( foundIt == m_ExtFileMap.end() ) {
			auto pSuffixObj = new suffixFileInfo();
			if ( shortSuffix == entireSuffix ) {
				// single ext
				pSuffixObj->singleExtList.push_back(pFInfo);
			} else {
				// multi ext
				// pSuffixObj->multiExtMap.insert( entireSuffix, pFInfo);
				pSuffixObj->multiExtMap[entireSuffix].push_back(pFInfo);
			}
			m_ExtFileMap.insert( shortSuffix, pSuffixObj);
		} else {
			auto pFoundSuffixObj = foundIt.value();
			if ( pFoundSuffixObj != nullptr ) {
				if ( shortSuffix == entireSuffix ) {
					// single ext
					pFoundSuffixObj->singleExtList.push_back( pFInfo );
				} else {
					// multi ext
					auto mutliObjIt = pFoundSuffixObj->multiExtMap.find(entireSuffix);
					if ( mutliObjIt == pFoundSuffixObj->multiExtMap.end() ) {
						pFoundSuffixObj->multiExtMap[entireSuffix].push_back(pFInfo);
					} else {
						mutliObjIt.value().push_back(pFInfo);
					}
				}
			}
		}
	}


}


//
// Search a Certain File
//
void MainWindow::on_pushButton_2_clicked()
{
	ui->searchResultList->setPlainText("");
	auto searchFileName = ui->lineEdit->text();
	searchFileName = searchFileName.trimmed();
	if ( searchFileName.isEmpty() ) {
		return;
	}

	
	
	// ui->searchResultList->setPlainText("");
}


void MainWindow::saveToRecord()
{
	static const QString NEWLINE("\n");

	QFileInfo visitPath( m_travelsalDirPath );
	auto shortFname = visitPath.fileName() + ".txt";
	// qDebug() << "shortFname = " << shortFname;
	
	QDir exe_dir( QCoreApplication::applicationDirPath() );
	QFileInfo dirInfo( exe_dir.absoluteFilePath(G_BASE_GEN_DIR) );
	// create a dir if necessary
	if ( !dirInfo.isDir() ) {
		auto successFlag = exe_dir.mkdir(G_BASE_GEN_DIR);
		if ( !successFlag ) {
			qDebug() << "create dir :  " << G_BASE_GEN_DIR <<  " Failed ";
			return;
		} 
	} 


	// qDebug() << "create file name = " << dirInfo.absoluteDir().absoluteFilePath(shortFname);
	QFile storedfile( QString( dirInfo.absoluteFilePath() + "/" + shortFname ) );
	storedfile.open( QIODevice::Truncate | QIODevice::WriteOnly );
	QFileInfo tmpPathInfo(m_travelsalDirPath);
	QString rootInfo = G_ROOT_STR + QString("%1\n").arg( tmpPathInfo.absoluteFilePath() );
	storedfile.write( rootInfo.toUtf8() );
	for ( auto it = m_ExtFileMap.begin(); it != m_ExtFileMap.end(); ++it ) 
	{
		auto ext = it.key();
		QString header = QString("\".%1\"\n").arg(ext);
		storedfile.write( header.toUtf8() );
		// qDebug() << QString("\".%1\"").arg(ext);
		qDebug() << "\"." << ext.toStdString().c_str() << "\"";

		suffixFileInfo* pFileInfo = it.value();
		if ( pFileInfo != nullptr ) {
			// QList<QFileInfo*>                     singleExtList;
			// QMap<QString, QList<QFileInfo*> >     multiExtMap;
			if ( !pFileInfo->singleExtList.empty() ) {
				QString header2 = QString("\t\".%1\" , count : %2\n").arg(ext).arg( pFileInfo->singleExtList.size() );
				storedfile.write( header2.toUtf8() );

				// qDebug() << QString("\t\".%1\" , count : %2").arg(ext).arg( pFileInfo->singleExtList.size() );
				qDebug() << "\t\"." <<  ext.toStdString().c_str() << "\" , count : " << pFileInfo->singleExtList.size();
			}

			int iidx = 1;
			for ( auto it2 = pFileInfo->singleExtList.begin(); it2 != pFileInfo->singleExtList.end(); ++it2, ++iidx )
			{
				QFileInfo* pfInfo = *it2;
				if ( pfInfo != nullptr ) {
					QString detail = QString("\t\t%1. | %2 | %3\n").arg(iidx).arg( pfInfo->fileName() ).arg( pfInfo->absoluteFilePath() );
					storedfile.write( detail.toUtf8() );
				}
			}

			if ( !pFileInfo->singleExtList.empty() ) {
				storedfile.write( NEWLINE.toUtf8() );
			}

			for ( auto it3 = pFileInfo->multiExtMap.begin(); it3 != pFileInfo->multiExtMap.end(); ++it3 ) \
			{
				auto multiKey = it3.key();
				auto &lst = it3.value();
				QString header3 = QString("\t\".%1\" , count : %2\n").arg(multiKey).arg( lst.size() );
				storedfile.write( header3.toUtf8() );

				// qDebug() << QString("\t\".%1\" , count : %2").arg(multiKey).arg( lst.size() );
				qDebug() << "\t\"." << multiKey.toStdString().c_str() << "\" , count : " << lst.size();

				iidx = 1;
				for ( auto it4 = lst.begin(); it4 != lst.end(); ++it4, ++iidx )
				{
					QFileInfo* pMutFInfo = *it4;
					if ( pMutFInfo != nullptr ) 
					{
						QString detail = QString("\t\t%1. | %2 | %3\n").arg(iidx).arg( pMutFInfo->fileName() ).arg( pMutFInfo->absoluteFilePath() );
						storedfile.write( detail.toUtf8() );
					}
				}
				storedfile.write( NEWLINE.toUtf8() );
			}

			qDebug() << "\n";

			storedfile.write( NEWLINE.toUtf8() );
		}

		
		// storedfile.write( NEWLINE.toUtf8() );
	}

	if ( !m_noExtFileList.empty() ) {
		storedfile.write( NEWLINE.toUtf8() );
		QString head = QString("\"%1\" , count : %2\n").arg(G_NO_EXT_FILE_FLAG).arg( m_noExtFileList.size() );
		storedfile.write( head.toUtf8() );
	}

	auto idxNoExt = 1;
	for( auto it = m_noExtFileList.begin(); it != m_noExtFileList.end(); ++it, ++idxNoExt )
	{
		QFileInfo* fInfo = *it;
		if ( fInfo != nullptr ) {
			QString detail = QString("\t%1. | %2 | %3\n").arg(idxNoExt).arg( fInfo->fileName() ).arg( fInfo->absoluteFilePath() );
			storedfile.write( detail.toUtf8() );
		}
	}

	storedfile.flush();
	storedfile.close();

}

void MainWindow::loadFromRecord(const QString& path)
{
	QString strRecordFilePath = QCoreApplication::applicationDirPath();
	if ( !strRecordFilePath.endsWith("/") ) {
		strRecordFilePath += "/";
	}
	strRecordFilePath += G_BASE_GEN_DIR;
	strRecordFilePath += "/";
	// strRecordFilePath += (fInfo.fileName() + ".txt");

	QString processPath = path;
	if ( processPath.endsWith("/") || processPath.endsWith("\\") ) {
		processPath = processPath.mid(0, processPath.length()-1 );
	}
	QFileInfo fInfo(processPath);
	strRecordFilePath += (fInfo.fileName() + ".txt");

	QFileInfo recordFileInfo(strRecordFilePath);
	if ( !recordFileInfo.isFile() ) {
		// strRecordFilePath
		ui->outputBox->setPlainText( QString(R"(
==================================================
[ERROR] : File <%1> is not existed
==================================================
)").arg( strRecordFilePath ) );
		return;
	}

	//
	// Existed
	//
	QFile recordFile(strRecordFilePath);
	if( !recordFile.open(QIODevice::ReadOnly) ) {
		ui->outputBox->setPlainText( QString(R"(
==================================================
[ERROR] : Open File <%1>    Failed
==================================================
)").arg( strRecordFilePath ) );
		return;
	}
	

	// qDebug() << "Start Read Time : " << QTime::currentTime();
	auto ba = recordFile.readAll();
	// qDebug() << "End   Read Time : " << QTime::currentTime();

	// qDebug() << "Start Sep Time : " << QTime::currentTime();
    //  QChar eol('\n');
    auto spBa = ba.split( '\n' );
	auto reachNoExtPart = false;
	QString visitExt;
	QString lastExt;
	quint64 lineNo = 1;
	for ( auto it = spBa.begin(); it != spBa.end(); ++it, ++lineNo ) {
		QString line = QString(*it);
		auto trimline = line.trimmed();
		if ( trimline.isEmpty() ) {
			continue;
		}

		// G_ROOT_STR
		// G_NO_EXT_FILE_FLAG
		// \t
		// \t\t
		auto idx1 = line.indexOf(G_ROOT_STR);
		if ( idx1 == 0 ) {
			QString rest = line.mid( G_ROOT_STR.length() );
			if( fInfo.absoluteFilePath() != rest ) {
				ui->outputBox->setPlainText( QString(R"(
==================================================
[ERROR] : RootPath is not equal to the file record
==================================================)") );
				break;
			} 
			// Status is OK
		} else if ( idx1 == -1 ) {
			if ( line.startsWith(G_TAB_1) ) {
				if ( !line.startsWith(G_TAB_2) ) {
					// Only 1 Tab
					if ( !reachNoExtPart ) {
						auto dotIdx = trimline.lastIndexOf("\"");
						auto quotestr = trimline.mid(0, dotIdx+1);
						auto innerQuote = quotestr.mid(2,quotestr.length()-3);
						lastExt = innerQuote;
					} else {
						auto part3Ary = trimline.split("|");
						QString abspath = part3Ary.at(2);
						abspath = abspath.trimmed();
						m_noExtFileList.push_back( new QFileInfo(abspath) );
					}
				} else {
					//  Start with 2 <Tab>s
					auto part3Ary = trimline.split("|");
					QString abspath = part3Ary.at(2);
					abspath = abspath.trimmed();
					auto nextDotIdx = lastExt.lastIndexOf(".");
					if ( nextDotIdx == -1 ) {
						// single
						auto foundIt = m_ExtFileMap.find( visitExt );
						if ( foundIt == m_ExtFileMap.end() ) {
							qDebug() << QString("[ERROR] : Can't found Single Ext %1 @ LINE : %2").arg(visitExt).arg(lineNo);
						} else {
							suffixFileInfo* sInfo = *foundIt;
							if ( sInfo != nullptr ) {
								sInfo->singleExtList.push_back( new QFileInfo(abspath) );
							} else {
								qDebug() << QString("[ERROR] : sInfo == nullptr @ LINE : %2").arg(lineNo);
							}
						}
					} else {
						// multi
						auto onlyExt = lastExt.mid( nextDotIdx + 1);
						auto foundIt = m_ExtFileMap.find( onlyExt );
						if ( foundIt == m_ExtFileMap.end() ) {
							qDebug() << QString("[ERROR] : Can't found MultiExt ext %1 @ LINE : %2").arg(lastExt).arg(lineNo);
						} else {
							suffixFileInfo* sInfo = *foundIt;
							if ( sInfo != nullptr ) {
								auto multiIt = sInfo->multiExtMap.find( lastExt );
								if ( multiIt == sInfo->multiExtMap.end() ) {
									sInfo->multiExtMap[lastExt].push_back( new QFileInfo(abspath) );
								} else {
									multiIt.value().push_back( new QFileInfo(abspath) );
								}
							} else {
								qDebug() << QString("[ERROR] : sInfo == nullptr @ LINE : %2").arg(lineNo);
							}
						}
					}
				}
			} else {
				if ( line.indexOf(G_NO_EXT_FILE_FLAG) != -1 ) {
					reachNoExtPart = true;
				} else {
					if ( line.indexOf("\"") == 0 ) {
						auto ext = line.mid(2, (line.length() - 3 ));
						visitExt = ext;
						suffixFileInfo* createNewObj = new suffixFileInfo;
						m_ExtFileMap.insert(ext,createNewObj);
					} else {
						qDebug() << "[ERROR] At Line : " << lineNo << " , Strange Version";
					}
				}
			}	
		}
	}
	// qDebug() << "line.count = " << spBa.size();
	// qDebug() << "End Sep Time : " << QTime::currentTime();


	
	//
	// Open Successful
	//


	
}

