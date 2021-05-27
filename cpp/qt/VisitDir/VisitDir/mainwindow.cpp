#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "visitThread.h"

#include <QDir>
#include <QFileInfoList>
#include <QDebug>
#include <QScrollBar>
#include <QFileInfo>
#include <QTime>


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


void MainWindow::on_pushButton_clicked()
{	
	/*
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
	*/


	//
	// Already running
	//
	if ( m_runningStatus == 1 ) {
		return;
	} 

	releaseMemory();
	// m_extensionFileMap.clear();
    ui->statusbar->showMessage("Running ... ");

	QString path2Visit = ui->visitPath->toPlainText();
	m_travelsalDirPath = path2Visit;
	// m_runningStatus != 0
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


void MainWindow::onVisitSomething(const QString &what, int type, int level)
{
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
	ui->outputBox->setPlainText(newstr);

    ui->statusbar->showMessage( QString("Done!! Elapsed Time : %1 ").arg(strUsedTime) );
	auto vBar = ui->outputBox->verticalScrollBar();
	if ( vBar != nullptr ) {
		auto max_Y = vBar->maximum();
		// qDebug() << "2. max_Y = " << max_Y;
		ui->outputBox->scroll(0, max_Y );
		ui->outputBox->update();
	}

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

