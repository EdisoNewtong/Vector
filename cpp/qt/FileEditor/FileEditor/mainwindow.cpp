#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QByteArray>
#include <QStringList>
#include <QVector>
#include <QFileDialog>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, plineEOLGrp( nullptr )
	, pfileEOLGrp( nullptr )
	, m_isCursorMoveCauseChanged( false )
	, m_isReadFileCauseChanged( false )
	, m_isForceMoveTextCursor( false ) 
{
    ui->setupUi(this);

	plineEOLGrp = new QButtonGroup( ui->lineEOL );
	plineEOLGrp->addButton( ui->eol_unix , 0);
	plineEOLGrp->addButton( ui->eol_win , 1);
	plineEOLGrp->addButton( ui->eol_mac , 2);
	plineEOLGrp->setExclusive( true );

	pfileEOLGrp = new QButtonGroup( ui->globalFileEOL );
	pfileEOLGrp->addButton( ui->f_eol_unix, 0 );
	pfileEOLGrp->addButton( ui->f_eol_win, 1 );
	pfileEOLGrp->addButton( ui->f_eol_mac, 2 );
	pfileEOLGrp->addButton( ui->f_eol_hybrid, 3 );
	pfileEOLGrp->setExclusive( true );


	connect( ui->plainTextEdit, SIGNAL( cursorPositionChanged() ), this, SLOT( textBoxCursor_changed() ) );
	connect( ui->plainTextEdit, SIGNAL( eolFlagDecide(int)  ),     this, SLOT( file_eol_decide(int) ) );

    connect( this->plineEOLGrp, SIGNAL( buttonToggled(int,bool) ), this, SLOT( line_EOL_changed(int,bool) ) );
    connect( this->pfileEOLGrp, SIGNAL( buttonToggled(int,bool) ), this, SLOT( file_line_EOL_changed(int,bool) ) );
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_loadBtn_clicked()
{
	// Load From File
	QString dirpath = ui->plainTextEdit->getOpenedFileDir();
	
	QString filename = QFileDialog::getOpenFileName(this, QString("Select a file to Load ... "),  (dirpath.isEmpty() ? QString("") : dirpath) );
	if ( !filename.isEmpty() ) {
		QFile fileToLoad(filename);

		if( fileToLoad.open( QIODevice::ReadOnly) ) {
			QByteArray contentOfFile = fileToLoad.readAll();
			if ( !(ui->plainTextEdit->processRawText( contentOfFile )) ) {
				showHintsMessage( ui->plainTextEdit->getErrorMsg(), 2, 5000);
			} else {
				showHintsMessage( QString("Load File Successful !"), 1, 3000);
			}
		} else {
			showHintsMessage( QString("Can't open File \"%1\" ").arg(filename), 2, 5000);
		}
	}

}

void MainWindow::on_rescanBtn_clicked()
{
	// Re Scan
	if ( ui->plainTextEdit->forceReadAgain() ) {
		showHintsMessage("Re-Scan Successful", 1, 3000);
	} else {
		showHintsMessage(ui->plainTextEdit->getErrorMsg(), 2, 3000);
	}
}


void MainWindow::on_forceRead_clicked()
{
	// Force Read from   plainTextEdit's  input
    if ( !(ui->plainTextEdit->processRawText( ui->plainTextEdit->toPlainText().toUtf8() )) ) {
		showHintsMessage(ui->plainTextEdit->getErrorMsg(), 2, 3000);
	} else {
		showHintsMessage( QString("Read from textBox Successful"), 1, 3000);
	}
}


void MainWindow::on_saveBtn_clicked()
{
	// Save File
	auto alreadyOpenedFile = ui->plainTextEdit->getOpenedFile();
	if ( alreadyOpenedFile.isEmpty() ) {
		showHintsMessage( QString("No loaded file yet !"), 2, 3000);
		return;
	}

	auto selectedEOLFlag = getSavedOption();
	QString errorMsg;
	auto saveRet =  ui->plainTextEdit->saveToFile(alreadyOpenedFile, selectedEOLFlag,errorMsg);
	if ( !saveRet ) {
		showHintsMessage(QString("[ERROR] : %1").arg(errorMsg), 2, 3000 );
	} else {
		showHintsMessage( QString("Save Done :)"), 1, 3000 );
	}
}



void MainWindow::on_saveasBtn_clicked()
{
	// Save As ... 

	QString dirpath    = ui->plainTextEdit->getOpenedFileDir();
	QString fileToSave = QFileDialog::getSaveFileName(this, QString("Save as ..."), (dirpath.isEmpty() ? QString() : dirpath) );
	if ( !( fileToSave.isEmpty() || fileToSave.isNull() ) ) {

		auto selectedEOLFlag = getSavedOption();
		QString errorMsg;
		auto saveRet =  ui->plainTextEdit->saveToFile(fileToSave, selectedEOLFlag ,errorMsg);

		if ( !saveRet ) {
			showHintsMessage(QString("[ERROR] : %1").arg(errorMsg), 2, 3000 );
		} else {
			showHintsMessage( QString("Save Done :)"), 1, 3000 );
		}
	} 

}



void MainWindow::textBoxCursor_changed()
{
	clearHighLight();

    QTextCursor currentCursor = ui->plainTextEdit->textCursor();
    int curLineNo = currentCursor.blockNumber(); // start from 0    rather than 1
    int qtPosition  = currentCursor.position();
	bool isAtBlockEnd = currentCursor.atBlockEnd();

	if ( !m_isForceMoveTextCursor ) {
		showHintsMessage( QString("[Cursor]   L:%1 , idx:%2").arg( (curLineNo+1 )).arg(qtPosition), 0, 3000 );
	}

	if ( !(ui->plainTextEdit->isProcessSuccess()) ) {
		// ui->hoveredLine->setHtml( QString("<font color=\"red\"> File has not been loaded yet.</font>") );
		ui->hoveredLine->setHtml( "" ); 
		return;
	}

	auto lineVec = ui->plainTextEdit->getLineList();
	
    int lineIdx = curLineNo;
	if ( lineIdx>=0   &&   lineIdx < lineVec.size() ) {
		LineInfo* curLineInfo = lineVec.at(lineIdx);
		if ( curLineInfo != nullptr ) {
			QVector< QPair<QString,bool> > hexLine;
			
            int foundIdx = -1;
			CharacterInfo foundCh;
			for ( int i = 0; i < curLineInfo->charList.size(); ++i ) {
				CharacterInfo chInfo = curLineInfo->charList.at(i);
				bool isResetMatched = false;
				bool isMatched = (chInfo.globalQtCharIdx == qtPosition);

				if (         !isMatched  
						&&   isAtBlockEnd
						&&   (curLineInfo->eolFlag == E_EOL_TYPE::E_CRLF)
						&&   chInfo.singleChar == QString("\n")
						&&   chInfo.isEOL ) {
					isMatched = true;
					isResetMatched = true;
				} 

				for( int j = 0; j < chInfo.utf8seq.size(); ++j ) {
					QString strHexCode;
					int chCode = static_cast<int>( chInfo.utf8seq.at(j) & 0xFF);
					strHexCode.setNum(chCode, 16);
					strHexCode = strHexCode.toUpper();
					strHexCode = strHexCode.rightJustified(2, '0');
					hexLine.push_back( qMakePair(strHexCode,isMatched) );
				}


                if ( isMatched && !isResetMatched ) {
                    foundIdx = i;
					foundCh = chInfo;
				} 
			}


			QString fmtStr;
			if ( !hexLine.empty() ) {
				for( int i = 0; i < hexLine.size(); ++i ) {
                    auto strHexValue = hexLine.at(i).first;
					auto bNeedSetColor = hexLine.at(i).second;
					fmtStr += "| ";

					if ( bNeedSetColor ) { fmtStr += "<font color=\"red\">"; }
                    fmtStr.push_back( strHexValue );
					if ( bNeedSetColor ) { fmtStr += "</font>"; }

					fmtStr += " ";
				}

				fmtStr += "|";
			} 
			ui->hoveredLine->setHtml( fmtStr );

			if ( foundIdx != -1 ) {
				ui->lineNo->setText( QString("%1").arg( lineIdx + 1) );
				ui->columnNo->setText( QString("%1").arg( foundIdx + 1) );
				ui->byteIdx->setText( QString("%1").arg( foundCh.globalByteIdx  ) );
				ui->charIdx->setText( QString("%1").arg( foundCh.globalCharIdx  ) );
                // ui->qtcharIdx->setText( QString("%1").arg( foundCh.globalQtCharIdx  ) );
                ui->qtcharIdx->setText( QString("%1").arg( qtPosition  ) );
				
				QString pickedCh( foundCh.utf8seq );
				auto ucs4CodeVec = pickedCh.toUcs4();
				if ( ucs4CodeVec.empty() ) {
					ui->unicodeValue->setText( " N/A " );
				} else {
					QString strnum;
					strnum.setNum( ucs4CodeVec.at(0), 16);
					strnum = strnum.toUpper();
					int len = strnum.size();
					if ( len % 2 == 1 ) {
						strnum = strnum.rightJustified(len+1, '0');
					}

					QString displayStr = foundCh.singleChar;
					if ( displayStr == QString("\r") ) {
						displayStr = "\\r";
					} else if ( displayStr == QString("\n") ) {
						displayStr = "\\n";
					} else if ( displayStr == QString("\t") ) {
						displayStr = "\\t";
					} else if ( displayStr == QString( QChar(32) ) ) {
						displayStr = "<SPACE>";
					}
					
					ui->unicodeValue->setText( QString("U+%1 : %2").arg(strnum).arg( displayStr )  );
				}

				if( !isAtBlockEnd ) {
					highLightCharacter( foundCh );
				}

            } else {
				ui->lineNo->setText(" N/A ");
				ui->columnNo->setText(" N/A ");
				ui->byteIdx->setText(" N/A ");
				ui->charIdx->setText(" N/A ");
				ui->qtcharIdx->setText(" N/A ");
				ui->unicodeValue->setText(" N/A ");
			} 

			pickEOLInfo( curLineInfo );

		} else {
			ui->hoveredLine->setHtml( QString("<font color=\"red\"> LineInfo is nullptr.</font>") );
		}
	} else {
		ui->hoveredLine->setHtml( QString("<font color=\"red\"> lineIdx : %1 is out of array's index range. </font>").arg(lineIdx) );
	}
}


void MainWindow::line_EOL_changed(int btnId, bool checked)
{
	// qDebug() << "Line => btnid = " << btnId << ", checked = " << checked;
	if ( m_isCursorMoveCauseChanged ) {
		// qDebug() << "Cursor Moved , return ";
		return;
	}

    QTextCursor currentCursor = ui->plainTextEdit->textCursor();
    int curLineNo = currentCursor.blockNumber(); // start from 0    rather than 1
	
	if ( checked ) {
		auto lineVec = ui->plainTextEdit->getLineList();
		if ( curLineNo>=0   &&   curLineNo < lineVec.size() ) {
            LineInfo* curLineInfo = lineVec.at(curLineNo);
			if ( curLineInfo != nullptr &&   curLineInfo->eolFlag != E_EOL_TYPE::E_NONE ) {
				E_EOL_TYPE newEOL = E_EOL_TYPE::E_NONE;
				if ( btnId == 0 ) {
					newEOL = E_EOL_TYPE::E_LF;
				} else if ( btnId == 1 ) {
					newEOL = E_EOL_TYPE::E_CRLF;
				} else if ( btnId == 2 ) {
					newEOL = E_EOL_TYPE::E_CR;
				}

				auto updateSucc = ui->plainTextEdit->updateEOL(curLineNo, curLineInfo->eolFlag , newEOL);
				if ( updateSucc ) {
					showHintsMessage("Update EOL Successful", 1, 3000);
				} else {
					showHintsMessage( ui->plainTextEdit->getErrorMsg(), 2, 3000);
				}
			}
		}
	} 

}


void MainWindow::file_line_EOL_changed(int btnId, bool  checked)
{
	Q_UNUSED(btnId)
	Q_UNUSED(checked)

    // qDebug() << "File => btnid = " << btnId << ", checked = " << checked;
	// if ( m_isReadFileCauseChanged ) {
	// 	qDebug() << "Read File Done , return ";
	// 	return;
	// }

}


void MainWindow::showHintsMessage( const QString& msg, int tag, int msec)
{
	if ( tag == 0 ) {
		// normal
		ui->statusbar->setStyleSheet( "QStatusBar { color: black; }" );
	} else if ( tag == 1 ) {
		// success
		ui->statusbar->setStyleSheet( "QStatusBar { color: black; background: #8fff82; }" );
	} else if ( tag == 2 ) {
		// fail
		ui->statusbar->setStyleSheet( "QStatusBar { color: red; }" );
	} else if ( tag == 3 ) {
		// warning
		ui->statusbar->setStyleSheet( "QStatusBar { color: #FFAA00; }" );
	} else {
		ui->statusbar->setStyleSheet("");
	}


	ui->statusbar->showMessage(msg, msec);
}



void MainWindow::highLightCharacter(const CharacterInfo& chInfo)
{
    QTextCursor currentCursor = ui->plainTextEdit->textCursor();

	QTextCursor selCursor(currentCursor);
	selCursor.setPosition( chInfo.globalQtCharIdx, QTextCursor::MoveAnchor);
	// selCursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor,  (chInfo.utf8seq.size() < 4 ? 1 : 2) );
	selCursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor,  1 );

	QTextEdit::ExtraSelection selection;
	selection.cursor = selCursor;
	selection.format.setBackground( QBrush(Qt::yellow) );

	QList<QTextEdit::ExtraSelection> lst;
	lst.push_back(selection);
	ui->plainTextEdit->setExtraSelections(lst);
}


void MainWindow::clearHighLight()
{
	QList<QTextEdit::ExtraSelection> lst;
	ui->plainTextEdit->setExtraSelections(lst);
}



void MainWindow::on_jmpBtn_clicked()
{
	if ( !(ui->plainTextEdit->isProcessSuccess()) ) {
		showHintsMessage( QString("Raw Buffer is not loaded successfullly !"), 2, 3000 );
		return;
	}

	QString strJmpPosition = ui->positionEdit->text();
	bool convertSuccFlag = false;
	int toJmpPos = strJmpPosition.toInt(&convertSuccFlag, 10);
	if ( !convertSuccFlag  ) {
		showHintsMessage( QString("\"%1\" can't convert to int ").arg(strJmpPosition), 2, 3000 );
		return;
	} else if ( toJmpPos < 0 ) {
		showHintsMessage( QString("%1 , negative number is not allowed ").arg(toJmpPos), 2, 3000 );
		return;
	}


	//
	// Valid number >=0
	//
	bool hasFound = false;
	bool exactedMatched = false;
	auto lineVec = ui->plainTextEdit->getLineList();

	if ( ui->byteChkBox->isChecked() ) {
		int qtCursor = -1;
		// Byte Index is Checked
		for ( int i = 0; i < lineVec.size();  ++i ) {
			LineInfo* curLineInfo = lineVec.at(i);
			if ( curLineInfo != nullptr ) {

				int passedJ = -1;
				for ( int j = 0; j < curLineInfo->charList.size(); ++j ) {
					CharacterInfo chInfo = curLineInfo->charList.at(j);
					if ( chInfo.globalByteIdx == toJmpPos ) {
						qtCursor = chInfo.globalQtCharIdx;
						hasFound = true;
						exactedMatched = true;
						break;
					} else if ( chInfo.globalByteIdx > toJmpPos ) {
						passedJ = j;
						break;
					}
				} // end inner for 

				if ( hasFound ) {
					break;
				} else {
					if ( passedJ != -1 ) {
						--passedJ;
						if ( passedJ >= 0 ) {
							qtCursor = curLineInfo->charList.at(passedJ).globalQtCharIdx;
							hasFound = true;
							break;
						} else {
							// lineVec.(i-1)
                            if ( (i-1) >= 0 ) {
								LineInfo* pPreviousLine = lineVec.at(i-1);
								if ( pPreviousLine != nullptr ) {
									if ( !pPreviousLine->charList.empty() ) {
										CharacterInfo preLineChInfo = pPreviousLine->charList.at( pPreviousLine->charList.size()-1 );
										qtCursor = preLineChInfo.globalQtCharIdx;
										hasFound = true;
										break;
									}
								}
							}
						}
					}
				}
			} 
		} // end outer for 

		if ( hasFound ) {
			if ( exactedMatched ) {
				m_isForceMoveTextCursor = true;

				showHintsMessage( QString("ByteIdx fulled matched, Jump Done") , 1, 3000);
				forceMoveTextCursor( qtCursor );

				m_isForceMoveTextCursor = false;
			} else {
				m_isForceMoveTextCursor = true;

				showHintsMessage( QString("ByteIdx to previous byte , Jump Done") , 3, 3000);
				forceMoveTextCursor( qtCursor );

				m_isForceMoveTextCursor = false;
			}
		} else {
			showHintsMessage( QString("Can't find byteIdx : %1").arg( toJmpPos ) , 2, 3000);
		}

	} else {
		//
		// Qt Cursor Checked 
		//
		int qtCursor = -1;
		for ( int i = 0; i < lineVec.size();  ++i ) {
			LineInfo* curLineInfo = lineVec.at(i);
			if ( curLineInfo != nullptr ) {

				int passedJ = -1;
				for ( int j = 0; j < curLineInfo->charList.size(); ++j ) {
					CharacterInfo chInfo = curLineInfo->charList.at(j);
					if ( chInfo.globalQtCharIdx == toJmpPos ) {
						qtCursor = chInfo.globalQtCharIdx;
						hasFound = true;
						exactedMatched = true;
						break;
					} else if ( chInfo.globalQtCharIdx > toJmpPos ) {
						passedJ = j;
						break;
					}
				} // end inner for 

				if ( hasFound ) {
					break;
				} else {
					if ( passedJ != -1 ) {
						--passedJ;
						if ( passedJ >= 0 ) {
							qtCursor = curLineInfo->charList.at(passedJ).globalQtCharIdx;
							hasFound = true;
							break;
						} else {
							// lineVec.(i-1)
                            if ( (i-1) >= 0 ) {
								LineInfo* pPreviousLine = lineVec.at(i-1);
								if ( pPreviousLine != nullptr ) {
									if ( !pPreviousLine->charList.empty() ) {
										CharacterInfo preLineChInfo = pPreviousLine->charList.at( pPreviousLine->charList.size()-1 );
										qtCursor = preLineChInfo.globalQtCharIdx;
										hasFound = true;
										break;
									}
								}
							}
						}
					}
				}
			} 
		} // end for

		if ( hasFound ) {
			if ( exactedMatched ) {
				m_isForceMoveTextCursor = true;

				showHintsMessage( QString("Qt CursorIndex fulled matched, Jump Done") , 1, 3000);
				forceMoveTextCursor( qtCursor );

				m_isForceMoveTextCursor = false;
			} else {
				m_isForceMoveTextCursor = true;

				showHintsMessage( QString("Qt CursorIndex to previous Cursor , Jump Done") , 3, 5000);
				forceMoveTextCursor( qtCursor );

				m_isForceMoveTextCursor = false;
			}
		} else {
			showHintsMessage( QString("Can't find Qt Index : %1").arg(toJmpPos) , 2, 3000);
		}

	}

}


void MainWindow::pickEOLInfo( LineInfo* pLineInfo )
{
	if ( pLineInfo != nullptr ) {
		m_isCursorMoveCauseChanged = true;

		if ( pLineInfo->eolFlag == E_EOL_TYPE::E_NONE ) {
            // qDebug() << "All UnChecked";
			ui->lineEOL->setEnabled( false );

			ui->eol_unix->setChecked(false);
			ui->eol_win->setChecked(false);
			ui->eol_mac->setChecked(false);
		} else if ( pLineInfo->eolFlag == E_EOL_TYPE::E_LF ) {
			ui->lineEOL->setEnabled( true );

			ui->eol_unix->setChecked(true);
			ui->eol_win->setChecked(false);
			ui->eol_mac->setChecked(false);
		} else if ( pLineInfo->eolFlag == E_EOL_TYPE::E_CRLF ) {
			ui->lineEOL->setEnabled( true );

			ui->eol_unix->setChecked(false);
			ui->eol_win->setChecked(true);
			ui->eol_mac->setChecked(false);
		} else {
			ui->lineEOL->setEnabled( true );
			// eol  :  \r
			ui->eol_unix->setChecked(false);
			ui->eol_win->setChecked(false);
			ui->eol_mac->setChecked(true);
		} 

		m_isCursorMoveCauseChanged = false;
	} else {

		ui->lineEOL->setEnabled( false );

		m_isCursorMoveCauseChanged = false;
	}
}


void MainWindow::file_eol_decide(int val)
{
	E_EOL_TYPE tp = static_cast<E_EOL_TYPE>(val);

	if ( tp == E_EOL_TYPE::E_NONE ) {
		ui->globalFileEOL->setEnabled( true );

		ui->f_eol_unix->setChecked(false);
		ui->f_eol_win->setChecked(false);
		ui->f_eol_mac->setChecked(false);
		ui->f_eol_hybrid->setChecked( true );

		// set Default as HyBrid
		showHintsMessage( QString("File.EOL is not decided! "), 3, 3000 );

	} else if ( tp == E_EOL_TYPE::E_LF ) {
		ui->globalFileEOL->setEnabled( true );

		ui->f_eol_unix->setChecked(true);
		ui->f_eol_win->setChecked(false);
		ui->f_eol_mac->setChecked(false);
		ui->f_eol_hybrid->setChecked( false );

	} else if ( tp == E_EOL_TYPE::E_CRLF ) {
		ui->globalFileEOL->setEnabled( true );

		ui->f_eol_unix->setChecked(false);
		ui->f_eol_win->setChecked(true);
		ui->f_eol_mac->setChecked(false);
		ui->f_eol_hybrid->setChecked( false );

	} else if ( tp == E_EOL_TYPE::E_CR ) {
		ui->globalFileEOL->setEnabled( true );

		ui->f_eol_unix->setChecked(false);
		ui->f_eol_win->setChecked(false);
		ui->f_eol_mac->setChecked(true);
		ui->f_eol_hybrid->setChecked( false );
	} else if ( tp == E_EOL_TYPE::E_HYBRID ) {
		ui->globalFileEOL->setEnabled( true );

		ui->f_eol_unix->setChecked(false);
		ui->f_eol_win->setChecked(false);
		ui->f_eol_mac->setChecked(false);
		ui->f_eol_hybrid->setChecked( true );

	} else {
        ui->globalFileEOL->setEnabled( false );
	}
}



void MainWindow::forceMoveTextCursor(int pos)
{
    QTextCursor cur2Move = ui->plainTextEdit->textCursor();
	cur2Move.setPosition(pos, QTextCursor::MoveAnchor);
	ui->plainTextEdit->setTextCursor( cur2Move );
}


E_EOL_TYPE MainWindow::getSavedOption()
{
	E_EOL_TYPE selectedEOLFlag = E_EOL_TYPE::E_LF;
	if ( ui->f_eol_unix->isChecked() ) {
		selectedEOLFlag = E_EOL_TYPE::E_LF;
	} else if ( ui->f_eol_win->isChecked() ) {
		selectedEOLFlag = E_EOL_TYPE::E_CRLF;
	} else if ( ui->f_eol_mac->isChecked() ) {
		selectedEOLFlag = E_EOL_TYPE::E_CR;
	} else if ( ui->f_eol_hybrid->isChecked() ) {
		selectedEOLFlag = E_EOL_TYPE::E_HYBRID;
	} 

	return selectedEOLFlag;
}
