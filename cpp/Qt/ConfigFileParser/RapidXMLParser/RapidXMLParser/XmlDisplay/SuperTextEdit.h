#ifndef SUPER_TEXT_EDIT_H
#define SUPER_TEXT_EDIT_H

#include <QObject>
#include <QPlainTextEdit>


// QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class QDragEnterEvent;
class QDragLeaveEvent;
class QDropEvent;
//  QT_END_NAMESPACE

class LineNumberArea;

//![codeeditordefinition]

class SuperTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    SuperTextEdit(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	
	// Drag/Drop Event
	// virtual void dragEnterEvent(QDragEnterEvent* event)  Q_DECL_OVERRIDE;
	// virtual void dragLeaveEvent(QDragLeaveEvent* event)  Q_DECL_OVERRIDE;
	virtual void dropEvent(QDropEvent* event)  Q_DECL_OVERRIDE;


private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
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

//![extraarea]

#endif

