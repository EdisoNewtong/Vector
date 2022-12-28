#ifndef SUPER_TEXT_EDIT_H
#define SUPER_TEXT_EDIT_H

#include <QObject>
#include <QPlainTextEdit>
#include <QByteArray>
#include <QString>
#include <QVector>


// QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class QDragEnterEvent;
class QDragLeaveEvent;
class QDropEvent;
class QRect;
class QSize;
//  QT_END_NAMESPACE

class LineNumberArea;
class EndOfLineArea;

//![codeeditordefinition]


enum E_EOL_TYPE
{
	E_NONE,
	E_LF,      // \n
	E_CRLF,    // \r\n
	E_CR,      // \r
	
	E_HYBRID,   // mix with \n  or  \r\n  or   \r
};

struct CharacterInfo
{
	QString     singleChar;
	QByteArray  utf8seq;

	int globalByteIdx;
	int globalCharIdx;
	int globalQtCharIdx;
	bool isEOL;
};


struct LineInfo
{
	QString content; // normal sequence
	E_EOL_TYPE eolFlag;

	QVector<CharacterInfo> charList; // without    | \r | \n | \r\n

	void reset() {
		content = "";
		eolFlag = E_EOL_TYPE::E_NONE;

		charList.clear();
	}
};






class SuperTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    SuperTextEdit(QWidget *parent = nullptr);
    virtual ~SuperTextEdit();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
	void eolAreaPaintEvent(QPaintEvent *event);

    int lineNumberAreaWidth();
	int eolAreaWidth();

	QString getErrorMsg();

	bool forceReadAgain();
    bool processRawText(const QByteArray& buffer);
	void buffer2Text();

	const QVector<LineInfo*> getLineList() const;
	bool isProcessSuccess();

	bool updateEOL(int lineIdx,  E_EOL_TYPE oldState, E_EOL_TYPE newState);


	QString getOpenedFile();
	QString getOpenedFileDir();

	bool saveToFile( const QString& filename,  E_EOL_TYPE eolFlag, QString& errorMsg);
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	
	// Drag/Drop Event
	// virtual void dragEnterEvent(QDragEnterEvent* event)  Q_DECL_OVERRIDE;
	// virtual void dragLeaveEvent(QDragLeaveEvent* event)  Q_DECL_OVERRIDE;
	virtual void dropEvent(QDropEvent* event)  Q_DECL_OVERRIDE;


	void clearLineInfo();
private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

public:
signals:
	void eolFlagDecide(int val);

private:
    QWidget *lineNumberArea;
    QWidget *newEOLArea;

	QByteArray           m_openedFileBuffer;
	QVector<LineInfo*>   m_allLines;
	bool                 m_processResult;
	QString              m_processError;
	QString              m_filePath;

	QByteArray           m_EOL_RN;
	QByteArray           m_EOL_R;
	QByteArray           m_EOL_N;
};
	
//![codeeditordefinition]
//![extraarea]

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(SuperTextEdit *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const Q_DECL_OVERRIDE {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    SuperTextEdit* codeEditor;
};


class EndOfLineArea : public QWidget
{
public:
    EndOfLineArea(SuperTextEdit *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const Q_DECL_OVERRIDE {
        return QSize(codeEditor->eolAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
        codeEditor->eolAreaPaintEvent(event);
    }

private:
    SuperTextEdit* codeEditor;
};

//![extraarea]

#endif