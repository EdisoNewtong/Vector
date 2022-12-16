#include <QtWidgets>

#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QResizeEvent>
#include <QDropEvent>
#include "SuperTextEdit.h"

//![constructor]

SuperTextEdit::SuperTextEdit(QWidget *parent /* = nullptr */) 
	: QPlainTextEdit(parent)
	, lineNumberArea( nullptr )
	, newEOLArea( nullptr )
	, m_openedFileBuffer()
	, m_allLines()
	, m_processResult( false )
	, m_processError()
	, m_filePath()
	, m_EOL_RN("\r\n")
	, m_EOL_R("\r")
	, m_EOL_N("\n")
{
    lineNumberArea = new LineNumberArea(this);
    newEOLArea     = new EndOfLineArea(this);
	// in able drag single file into this widget , fast open it and show it's content
	setAcceptDrops(true);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    // connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
	clearLineInfo();

    // highlightCurrentLine();
}

//![constructor]


// virtual 
SuperTextEdit::~SuperTextEdit()
{
	clearLineInfo();

}



//![extraAreaWidth]

int SuperTextEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 5 + fontMetrics().width(QLatin1Char('9')) * digits;
    return space;
}


int SuperTextEdit::eolAreaWidth()
{
	return 50;
}


//![extraAreaWidth]

//![slotUpdateExtraAreaWidth]

void SuperTextEdit::updateLineNumberAreaWidth(int newBlockCount)
{
    Q_UNUSED(newBlockCount)
    setViewportMargins( lineNumberAreaWidth(), 0, eolAreaWidth(), 0 );
}

//![slotUpdateExtraAreaWidth]

//![slotUpdateRequest]

void SuperTextEdit::updateLineNumberArea(const QRect &rect, int dy)
{

    if ( dy ) {
        lineNumberArea->scroll(0, dy);
		newEOLArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
        newEOLArea->update( 0, rect.y(), eolAreaWidth(), rect.height());
    }

    if ( rect.contains( viewport()->rect() ) ) {
		updateLineNumberAreaWidth(0);
    }
}

//![slotUpdateRequest]

//![resizeEvent]

void SuperTextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();

	QRect rect1(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height());
    lineNumberArea->setGeometry( rect1 );

	// Core Core Core :
	//    When the verticalBar is shown, it will occur a certain space  ( e->size() will reduce the vBar->width() )
    QRect rect2(cr.left() + lineNumberAreaWidth() + e->size().width(), cr.top(), eolAreaWidth(), cr.height());
    newEOLArea->setGeometry( rect2 );
}

//![resizeEvent]

//![cursorPositionChanged]

void SuperTextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

//![cursorPositionChanged]

//![extraAreaPaintEvent_0]

void SuperTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

//![extraAreaPaintEvent_0]

//![extraAreaPaintEvent_1]
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = static_cast<int>( blockBoundingGeometry(block).translated(contentOffset()).top() );
    int bottom = top + static_cast<int>( blockBoundingRect(block).height() );
//![extraAreaPaintEvent_1]

//![extraAreaPaintEvent_2]

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        } 

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>( blockBoundingRect(block).height() );
        ++blockNumber;
    }
}
//![extraAreaPaintEvent_2]



void SuperTextEdit::eolAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter( newEOLArea );
    painter.fillRect( event->rect(), Qt::lightGray );

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = static_cast<int>( blockBoundingGeometry(block).translated(contentOffset()).top() );
    int bottom = top + static_cast<int>( blockBoundingRect(block).height() );

	if ( m_processResult ) {

		QTextCursor currentCursor = this->textCursor();

		while (block.isValid() && top <= event->rect().bottom()) {
			if (block.isVisible() && bottom >= event->rect().top()) {
				if ( blockNumber>=0 &&  blockNumber < m_allLines.size() ) {
					LineInfo* pLineInfo = m_allLines.at(blockNumber);
					if ( pLineInfo != nullptr ) {
						if ( pLineInfo->eolFlag !=  E_EOL_TYPE::E_NONE ) {
							if ( currentCursor.atBlockEnd() &&   (currentCursor.blockNumber() ==  blockNumber)  ) {
								painter.setPen(Qt::red);
							} else {
								painter.setPen(Qt::black);
							}

							QString eolstr;
							if ( pLineInfo->eolFlag == E_EOL_TYPE::E_LF ) {
								eolstr = "\\n";
							} else if ( pLineInfo->eolFlag == E_EOL_TYPE::E_CRLF ) {
								eolstr = "\\r\\n";
							} else {
								eolstr = "\\r";
							}
							painter.drawText(0, top, eolAreaWidth(), fontMetrics().height(),
											 Qt::AlignLeft, eolstr );
						}

					}
				}

			}

			block = block.next();
			top = bottom;
			bottom = top + static_cast<int>( blockBoundingRect(block).height() );
			++blockNumber;
		}
	}

}

// virtual 
void SuperTextEdit::dropEvent(QDropEvent* event)  // Q_DECL_OVERRIDE;
{
	// qDebug() << "Drop ";
    QPlainTextEdit::dropEvent(event);

	auto btnState = event->mouseButtons();
    qDebug() << "Mouse BtnState = " << btnState;

    bool hasLeftButtonPressed = true;
// #ifdef Q_OS_WIN
//     // hasLeftButtonPressed = ( (btnState & Qt::LeftButton)!=0 );
//     hasLeftButtonPressed = true;
// #elif defined(Q_OS_MAC)
//     (void)btnState;
//     hasLeftButtonPressed = true;
// #else
//     hasLeftButtonPressed = true;
// #endif


	if(  hasLeftButtonPressed ) {
		auto dropData = event->mimeData();
        if( dropData!=nullptr && dropData->hasUrls() ) {
            const auto& url_list = dropData->urls();
			if( url_list.size() !=1 ) {
				qDebug() << "Only Support Single File Drag/Drop";
			} else {
				// == 1
				auto url_file = url_list.at(0);
				QString localFileName = url_file.toLocalFile();
				QFileInfo fi(localFileName);
				qDebug() << "isSymLink = " << fi.isSymLink();
				if( fi.isFile() ) {
					QString fPath = fi.isSymLink() ? fi.symLinkTarget() : localFileName;
					m_filePath = fPath;

					QFile readedFile(fPath);
					if( readedFile.open( QIODevice::ReadOnly) ) {
						QByteArray contentOfFile = readedFile.readAll();
						processRawText(contentOfFile);
						// int fileSize = readedFile.size();

						// read the info and set the content
						QString strContent(contentOfFile);
						this->setPlainText(strContent);
					}
				} else {
					qDebug() << "Only Support Single File , Don't Support Dir/Other type ";
				}
			}
		}
    } else {
        qDebug() << "Not Mouse-Left-Released ";
    }
}


bool SuperTextEdit::processRawText(const QByteArray& buffer)
{
	static const QString SC_CR( QChar('\r') );
	static const QString SC_LF( QChar('\n') );

	QByteArray backupBuf( buffer );
	m_openedFileBuffer.clear();
	clearLineInfo();
	m_openedFileBuffer = backupBuf;
	m_processError = "";

	if ( m_openedFileBuffer.isEmpty() ) { 
		// m_processError = "Empty File Buffer";
		m_processResult = true;
        return m_processResult;
	}


    E_EOL_TYPE decideEolFlag = E_EOL_TYPE::E_NONE;

	LineInfo line;
	line.reset();

	int gCharIdx = 0;
	int gQtCharIdx = 0;
	bool meetError = false;
	int bufSz = m_openedFileBuffer.size();
	for( int i = 0; i < m_openedFileBuffer.size(); ++i  )
	{
		QByteArray ba;
		QString    validCharacter;
		int        delta = 0;

		bool isValidCharacter = false;

		for( int j = i; j < (i+4); ++j )
		{
			if ( j < bufSz ) {
				ba.push_back(  m_openedFileBuffer.at(j) );

				QString bastr(ba);
				auto ucs4CodeVec = bastr.toUcs4();

				auto isValidCh = true;
				if ( ucs4CodeVec.empty() ) {
					isValidCharacter = false;
					break;
				} else {

					for( int k = 0; k < ucs4CodeVec.size(); ++k ) {
						if ( QChar(ucs4CodeVec.at(k) ) == QChar::ReplacementCharacter ) {
							isValidCh = false;
							break;
						}
					}

					if ( isValidCh ) {
						delta = (j - i);
						isValidCharacter = true;
						validCharacter = bastr;
						break;
					}
				}
			} else {
				isValidCharacter = false;
			}
		} // end  [0~4)
		

		if ( isValidCharacter ) {
			bool bIsEOLFlag = false;
			if ( delta == 0 ) {   // Only 1 byte
				if ( validCharacter == SC_CR ) { // == '\r'
					if ( (i + 1) < bufSz ) {
						if ( QString( QChar( m_openedFileBuffer.at(i+1) ) ) == SC_LF ) { // == '\n'
							// always merge  "\r\n" as 1 byte: "\n"																					 
							line.eolFlag = E_EOL_TYPE::E_CRLF;
						} else {
							line.eolFlag = E_EOL_TYPE::E_CR;
						}
					} else {
						line.eolFlag = E_EOL_TYPE::E_CR; // '\r' is the <LAST> char of all the buffer
					}

					if ( decideEolFlag == E_EOL_TYPE::E_NONE ) {
						decideEolFlag = line.eolFlag;
					} else {
						if ( decideEolFlag != E_EOL_TYPE::E_HYBRID ) {
							if ( decideEolFlag != line.eolFlag ) {
							 	decideEolFlag = E_EOL_TYPE::E_HYBRID;
							}
						}
					}


					bIsEOLFlag = true;

					CharacterInfo chInfo1;
					chInfo1.singleChar = validCharacter;  // \r
					chInfo1.utf8seq    = ba;    
					chInfo1.globalByteIdx = i;
					chInfo1.globalCharIdx = gCharIdx++;
					chInfo1.globalQtCharIdx = gQtCharIdx++;
					chInfo1.isEOL = true;
                    line.charList.push_back( chInfo1 );

					if ( line.eolFlag == E_EOL_TYPE::E_CRLF ) {
						CharacterInfo chInfo2;
						chInfo2.singleChar = SC_LF;
						chInfo2.utf8seq    = QByteArray(1, '\n');
						chInfo2.globalByteIdx = (i+1);
						chInfo2.globalCharIdx = (gCharIdx-1);      // make '\n' closely after '\r'  as ( bind into one group (\r\n) )the same value of both gCharIdx and gQtCharIdx
						chInfo2.globalQtCharIdx = (gQtCharIdx-1);
						chInfo2.isEOL = true;

						line.charList.push_back( chInfo2 );
					}

				} else if ( validCharacter == SC_LF ) { // == '\n'
					line.eolFlag = E_EOL_TYPE::E_LF;

					if ( decideEolFlag == E_EOL_TYPE::E_NONE ) {
						decideEolFlag = line.eolFlag;
					} else {
						if ( decideEolFlag != E_EOL_TYPE::E_HYBRID ) {
							if ( decideEolFlag != line.eolFlag ) {
							 	decideEolFlag = E_EOL_TYPE::E_HYBRID;
							}
						}
					}


					bIsEOLFlag = true;

					CharacterInfo chInfo;
					chInfo.singleChar = validCharacter;
					chInfo.utf8seq    = ba;
					chInfo.globalByteIdx = i;
					chInfo.globalCharIdx = gCharIdx++;
					chInfo.globalQtCharIdx = gQtCharIdx++;
					chInfo.isEOL = true;

					line.charList.push_back( chInfo );

				} else {
					line.content += validCharacter;

					CharacterInfo chInfo;
					chInfo.singleChar = validCharacter;
					chInfo.utf8seq    = ba;
					chInfo.globalByteIdx = i;
					chInfo.globalCharIdx = gCharIdx++;
					chInfo.globalQtCharIdx = gQtCharIdx++;
					chInfo.isEOL = false;

					line.charList.push_back( chInfo );
				}
			} else {
				// multi bytes
				line.content += validCharacter;

				CharacterInfo chInfo;
				chInfo.singleChar = validCharacter;
				chInfo.utf8seq    = ba;
				chInfo.globalByteIdx = i;
				chInfo.globalCharIdx = gCharIdx++;
				chInfo.globalQtCharIdx = gQtCharIdx;
				chInfo.isEOL = false;

				if ( ba.size() == 4 ) {
					gQtCharIdx += 2;
				} else {
					gQtCharIdx += 1;
				}
				line.charList.push_back( chInfo );
			}


			if ( bIsEOLFlag )  {
				if ( line.eolFlag == E_EOL_TYPE::E_CRLF ) {
					// \r\n
					delta = 1;
				} 

				LineInfo* pNewLine = new LineInfo;
				*pNewLine = line;
                m_allLines.push_back( pNewLine );

				line.reset();
			} else {
				if ( (i + delta) == (bufSz-1) ) { // reach the leader byte of last character
					LineInfo* pNewLine = new LineInfo;
					*pNewLine = line;
					m_allLines.push_back( pNewLine );

					line.reset();
				}
			}

			i += delta;
		} else {

			QString strnum;
            strnum.setNum( static_cast<int>( m_openedFileBuffer.at(i) & 0xFF ) ,16);
			strnum = strnum.toUpper();

			m_processError = QString("Unprintable char @idx:%1, 0x%2").arg(i).arg(  strnum );
			meetError = true;
			break; // break outter loop
		}
	}

	emit eolFlagDecide( static_cast<int>( decideEolFlag) );

	m_processResult = !meetError;
	return m_processResult;
}


void SuperTextEdit::clearLineInfo()
{
	for( auto& pLineInfo : m_allLines ) {
		if ( pLineInfo != nullptr ) {
			delete pLineInfo;
			pLineInfo = nullptr;
		}
	}

	m_allLines.clear();
}


const QVector<LineInfo*> SuperTextEdit::getLineList() const
{
	return m_allLines;
}


bool SuperTextEdit::isProcessSuccess()
{
	return m_processResult;
}


bool SuperTextEdit::forceReadAgain()
{
	if ( m_openedFileBuffer.isEmpty() ) {
		m_processResult = false;
		m_processError = "File buffer is <Empty>";
		return m_processResult;
	}

	return processRawText( m_openedFileBuffer );
}



QString SuperTextEdit::getErrorMsg()
{
	return m_processError;
}


bool SuperTextEdit::updateEOL(int lineIdx, E_EOL_TYPE oldState, E_EOL_TYPE newState)
{
	QByteArray rep;
	if ( newState == E_EOL_TYPE::E_LF ) {
		rep = m_EOL_N; // \n
	} else if ( newState == E_EOL_TYPE::E_CRLF ) {
		rep = m_EOL_RN; // \r\n
	} else {
		rep = m_EOL_R; // \r
	}


	bool meetError = false;
	if ( oldState == newState ) {
		// Do nothing
		return true;
	}


	if ( lineIdx>=0   &&    lineIdx < m_allLines.size() ) {
		LineInfo* pLineInfo = m_allLines.at(lineIdx);
		if ( pLineInfo!= nullptr ) {
			if ( pLineInfo->eolFlag == oldState ) {
				int bufSz = m_openedFileBuffer.size();
				int chSz = pLineInfo->charList.size();

				if ( oldState == E_CRLF ) {
					// \r\n
					if ( chSz >= 2 ) {
						auto chlast2 = pLineInfo->charList.at(chSz-2);
						auto chlast = pLineInfo->charList.at(chSz-1);

						if (    (chlast2.globalByteIdx>=0   &&  chlast2.globalByteIdx < bufSz) 
							 && (chlast.globalByteIdx>=0    &&  chlast.globalByteIdx  < bufSz) ) {
							if (    m_openedFileBuffer.at(chlast2.globalByteIdx) == '\r'
								&&  m_openedFileBuffer.at(chlast.globalByteIdx)  == '\n' ) {
								// Do
								m_openedFileBuffer = m_openedFileBuffer.replace( chlast2.globalByteIdx, 2, rep);
							} else {
								m_processError = QString("buffer[%1] not Matched for \\r\\n").arg(chlast2.globalByteIdx);
								meetError = true;
							}
						} else {
							// ERROR
							m_processError = QString("One of index [%1, %2] is out of buffer's range \\r\\n").arg(chlast2.globalByteIdx).arg( chlast.globalByteIdx );
							meetError = true;
						}
					} else {
						// ERROR
						m_processError = QString("chSz:%1 is less than 2(\\r\\n) ").arg( chSz );
						meetError = true;
					}
				} else  {
					// 1 byte  ,  \r or \n
					if (  chSz >= 1 ) {
						auto chlast  = pLineInfo->charList.at(chSz-1);
						if ( chlast.globalByteIdx>=0   && chlast.globalByteIdx < bufSz ) {
							char chRn = m_openedFileBuffer.at(chlast.globalByteIdx);
							auto isEqual =   (oldState == E_EOL_TYPE::E_LF) 
										   ? ( chRn == QString("\n") ) 
										   : ( chRn == QString("\r") );
							if ( isEqual ) {
								m_openedFileBuffer = m_openedFileBuffer.replace( chlast.globalByteIdx, 1,  rep );
							} else {
								// ERROR
								m_processError = QString("Mismatched : get %1 , expected : %2")
									             .arg( chRn == '\r' ? QString("\\r") : (chRn == '\n' ? QString("\\n") : QString( QChar(chRn) )) )
									             .arg( oldState == E_EOL_TYPE::E_LF ? QString("\\n") : QString("\\r") );
								meetError = true;
							}
						} else {
							m_processError = QString("index [%1] is out of buffer's range \\r\\n").arg( chlast.globalByteIdx );
							meetError = true;
						}
					} else {
						m_processError = QString("chSz:%1 is less than 1 (\\r or \\n).").arg( chSz );
						meetError = true;
					}
				}
			} else {
				// ERROR
				m_processError = QString("old EOL type is not matched for the current line's EOL. ");
				meetError = true;
			}
		} else {
			m_processError = QString("pLineInfo is a nullptr");
			meetError = true;
		}
	} else {
		m_processError = QString("lineNo:%1 is out of range").arg( lineIdx+1 );
		meetError = true;
	}

	if ( !meetError ) {
		meetError = !forceReadAgain();
	}

	return !meetError;
}



QString SuperTextEdit::getOpenedFileDir()
{
	if ( m_filePath.isEmpty() || m_filePath.isNull() ) {
		return QString("");
	}

	QFileInfo fi(m_filePath);
	return fi.dir().path();
}


QString SuperTextEdit::getOpenedFile()
{
	return m_filePath;
}


bool SuperTextEdit::saveToFile(const QString& filename, E_EOL_TYPE eolFlag, QString& errorMsg)
{
	if ( eolFlag == E_EOL_TYPE::E_NONE ) {
		errorMsg = "Unknown arg.value : E_NONE inside saveToFile(...)";
		return false;
	}


	QByteArray modifiedBa;
	if ( eolFlag != E_EOL_TYPE::E_HYBRID ) {
		int sz = m_openedFileBuffer.size();
		for( int i = 0; i < sz; ++i )
		{
			if ( m_openedFileBuffer[i] == '\r' ) {
				bool isSpecialFlag = false;
				if ( (i+1) < sz ) {
					if ( modifiedBa[i+1] == '\n' ) {
						isSpecialFlag = true;
					} 
				} 

				if ( eolFlag == E_EOL_TYPE::E_LF ) {
					// \r     ==>  \n
					modifiedBa.push_back( '\n' );
				} else if ( eolFlag == E_EOL_TYPE::E_CRLF ) {
					// \r     ==>  \r\n
					modifiedBa.push_back( '\r' );
					modifiedBa.push_back( '\n' );
				} else {
					//   \r   ==>   \r
					modifiedBa.push_back( '\r' );
				}

				if ( isSpecialFlag ) {
					i += 1; // advance +1
				}

			} else if ( m_openedFileBuffer[i] == '\n' ) {

				if ( eolFlag == E_EOL_TYPE::E_LF ) {
					// \n     ==>  \n
					modifiedBa.push_back( '\n' );
				} else if ( eolFlag == E_EOL_TYPE::E_CRLF ) {
					// \n     ==>  \r\n
					modifiedBa.push_back( '\r' );
					modifiedBa.push_back( '\n' );
				} else {
					//   \n   ==>   \r
					modifiedBa.push_back( '\r' );
				}

			} else {
				// others 
				modifiedBa.push_back( m_openedFileBuffer[i] );
			}
		}
	} else {
		modifiedBa = m_openedFileBuffer;
	}



	QFile file(filename);
	if ( !file.open( QIODevice::WriteOnly ) ) {
		errorMsg = QString("Can't Open file : %1").arg(filename);
		return false;
	} 

	// qint64 QIODevice::write(const QByteArray &byteArray)
	int    basz = modifiedBa.size();
	qint64 writtenBytes = file.write( modifiedBa );
	if ( writtenBytes != ( static_cast<qint64>( basz ) ) ) {
		int delta = abs( basz - static_cast<int>(writtenBytes) ) ;
		errorMsg = QString("Missing %1 bytes to wrote").arg( delta );
		file.flush();
		return false;
	}  

	auto ret = file.flush();
	if ( !ret ) {
		errorMsg = QString("Flush File %1 failed").arg( filename );
	}
	return ret;

}
