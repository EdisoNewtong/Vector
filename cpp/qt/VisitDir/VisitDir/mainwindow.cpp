#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "visitThread.h"

#include <QDir>
#include <QFileInfoList>
#include <QDebug>
#include <QScrollBar>
#include <QFileInfo>
#include <QTime>

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
// #include <QJsonDocument>

static constexpr bool G_SHOW_DETAIL = false;

static const QString G_BASE_GEN_DIR("Travelsal_Log");


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, m_travelsalDirPath("")
	// , m_runningThread(nullptr)
	, m_runningStatus(0)
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
	loadFromJson(dirPath);
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
	ui->outputBox->setPlainText(QString(R"(--------------------------------------------------
Begin Travelsal Dir  => %1
--------------------------------------------------)").arg(path2Visit) );
	m_runningStatus = 1;

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
			// m_dirList.push_back(what);
		} else if ( type == 1 ) {
			// m_noneExtensionFileList
			processFileBySuffix(what);
			// visitTpName = "File";
		} else {
			// qDebug() << "[EdisonLog] : Strange File Type ?? ";
			// visitTpName = "Unknown-File";
		}

		// auto newstr = oldstr + "\n" + QString("%1-<%2> : %3").arg(level).arg(visitTpName).arg(what);
		// ui->outputBox->setPlainText(newstr);
	} else {
		
	}
}



void MainWindow::onVisitEntireDirDone()
{
	auto oldstr = ui->outputBox->toPlainText();

	auto duringms = m_timer.elapsed();
	qDebug() << "End   Time : " << QTime::currentTime();
	// qDebug() << "m_extensionFileMap.size() = " << m_extensionFileMap.size();
	// auto itcpp = m_extensionFileMap.find("cpp");
	// if ( itcpp != m_extensionFileMap.end() ) {
	// 	qDebug() << "OK     \".cpp\"	 count = " << itcpp.value();
	// } else {
	// 	qDebug() << "Sorry. \".cpp\"	 count = 0";
	// }

	// duringms = 3600000LL; // Test  only
    // qDebug() << "during     ms = " << (duringms);
    // qDebug() << "during sec(s) = " << (duringms / 1000.0f);

	auto intSecs = duringms / 1000L;
	QString strUsedTime;
	int hours = static_cast<int>(intSecs / 3600);
	int mins = (intSecs / 60) % 60;
	int secs = intSecs % 60;

	// qDebug() << "intSecs = " << intSecs << endl
	// 	     << "hours   = " << hours   << endl
	// 		 << "mins    = " << mins    << endl
	// 		 << "secs    = " << secs    << endl
	// 		 << endl;
	
	// float fnum = intSecs - (duringms / 1000.0f); //<< endl
	// qDebug() << "fnum = " << fnum;
	QString strFloatPart; 

	strFloatPart.setNum( (duringms / 1000.0f) - intSecs );
	qDebug() << "strFloatPart = " << strFloatPart;
	int idxOfDot = strFloatPart.indexOf(".");
	if ( idxOfDot != -1 ) {
		strFloatPart = strFloatPart.mid(idxOfDot+1);
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



	auto newstr = oldstr + "\n" + QString(R"(--------------------------------------------------
End Travelsal Dir  => %1 , Used  %2 seconds
--------------------------------------------------

)").arg(m_travelsalDirPath).arg(strUsedTime);

	// ui->outputBox->setPlainText(newstr);

    ui->statusbar->showMessage( QString("Done!!     Elapsed Time  =>    %1 ").arg(strUsedTime) );




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

	//
	// save to json file
	//
	//
	QJsonObject   rootObj;
	// 
	QJsonArray    noExtFileList;
	for ( auto it = m_noExtFileList.begin(); it != m_noExtFileList.end(); ++it ) {
		QFileInfo* pfInfo = *it;
		if ( pfInfo != nullptr ) {
			QJsonObject noExtObj;
			noExtObj.insert("FileName" , QJsonValue( pfInfo->fileName() ) );
			noExtObj.insert("FileDir"  , QJsonValue( pfInfo->path()) );
			noExtObj.insert("AbsFileName"  , QJsonValue( pfInfo->absoluteFilePath() ) );
			noExtFileList.push_back( noExtObj );
		}
	}
	rootObj.insert("<noExtFileList>", noExtFileList);

	/*
	for ( auto it = m_ExtFileMap.begin(); it != m_ExtFileMap.end(); ++it )
	{
		QJsonObject extObj;
		auto key = it.key();
		auto obj = it.value();
		auto dotWithKey = QString(".") + key;
		
		if ( obj != nullptr ) {
			QJsonArray sigArray;
			for ( auto sigIt =  obj->singleExtList.begin(); sigIt != obj->singleExtList.end(); ++sigIt ) {
				QFileInfo* pfInfo = *sigIt;
				if ( pfInfo != nullptr ) {
					QJsonObject sigObj;

					sigObj.insert("FileName" , QJsonValue( pfInfo->fileName() ) );
					sigObj.insert("FileDir"  , QJsonValue( pfInfo->path()) );
					sigObj.insert("AbsFileName"  , QJsonValue( pfInfo->absoluteFilePath() ) );

					sigArray.push_back( sigObj );
				}
			} 
			extObj.insert(dotWithKey, sigArray);

			
			for ( auto mulIt = obj->multiExtMap.begin(); mulIt != obj->multiExtMap.end(); ++mulIt ) {
				auto mtkey = mulIt.key();
				auto mtlst = mulIt.value();
				auto dotmtkey = QString(".") + mtkey;
				QJsonArray mtAry;
				for ( auto itemIt = mtlst.begin(); itemIt != mtlst.end(); ++itemIt ) {
					QFileInfo* pfInfo = *itemIt;
					if ( pfInfo!= nullptr ) {
						QJsonObject mulItem;

						mulItem.insert("FileName" , QJsonValue( pfInfo->fileName() ) );
						mulItem.insert("FileDir"  , QJsonValue( pfInfo->path()) );
						mulItem.insert("AbsFileName"  , QJsonValue( pfInfo->absoluteFilePath() ) );

						mtAry.push_back( mulItem );
					}
				}
				extObj.insert(dotmtkey, mtAry);
				
			}
			
			rootObj.insert(key, extObj);
		}

	}
	*/

	QJsonDocument jsonDoc(rootObj);
	saveToJson(jsonDoc);
	qDebug() << "==================================================";
	qDebug() << "==> After Write  : Done ";
	qDebug() << "==================================================";

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


void MainWindow::saveToJson(QJsonDocument& doc)
{
	QFileInfo visitPath(m_travelsalDirPath);
	auto shortFname = visitPath.fileName() + ".json";
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

	// QJsonObject obj;
	// obj.insert("aaa", QJsonValue(true) );
	// obj.insert("bbb", QJsonValue() );
	// QJsonDocument doc(obj);


	// qDebug() << "create file name = " << dirInfo.absoluteDir().absoluteFilePath(shortFname);
	QFile storedfile( QString( dirInfo.absoluteFilePath() + "/" + shortFname ) );
	storedfile.open( QIODevice::Truncate | QIODevice::WriteOnly );
	storedfile.write( doc.toJson() );
	storedfile.flush();
	storedfile.close();

}

void MainWindow::loadFromJson(const QString& path)
{
	Q_UNUSED(path)
}

