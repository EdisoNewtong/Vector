#include <QtWidgets>

#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QDropEvent>
#include "SuperTextEdit.h"

//![constructor]

SuperTextEdit::SuperTextEdit(QWidget *parent) : QPlainTextEdit(parent)
{
    const auto deltaNumberWidget = 2;

    lineNumberArea = new LineNumberArea(this);
    auto fntNumber = lineNumberArea->font();
    auto ps = fntNumber.pointSize();
    if ( ps != -1 ) {
        // specified by point unit
        fntNumber.setPointSize( ps + deltaNumberWidget );
    } else {
        // specified by pixel unit
        fntNumber.setPixelSize( fntNumber.pixelSize() + deltaNumberWidget );
    }
    lineNumberArea->setFont( fntNumber );


	// in able drag single file into this widget , fast open it and show it's content
	setAcceptDrops(true);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    // connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    // highlightCurrentLine();
}

//![constructor]

//![extraAreaWidth]

int SuperTextEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    // int space = 5 + fontMetrics().width(QLatin1Char('9')) * digits;
    int space = 5 + lineNumberArea->fontMetrics().horizontalAdvance( QLatin1Char('9')) * digits;
    return space;
}

//![extraAreaWidth]

//![slotUpdateExtraAreaWidth]

void SuperTextEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

//![slotUpdateExtraAreaWidth]

//![slotUpdateRequest]

void SuperTextEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
	}
}

//![slotUpdateRequest]

//![resizeEvent]

void SuperTextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
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
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
//![extraAreaPaintEvent_1]

//![extraAreaPaintEvent_2]
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), lineNumberArea->fontMetrics().height(),
                             Qt::AlignCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
//![extraAreaPaintEvent_2]



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
					QFile readedFile(fPath);
					if( readedFile.open( QIODevice::ReadOnly) ) {
						QByteArray contentOfFile = readedFile.readAll();
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

