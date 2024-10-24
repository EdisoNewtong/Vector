#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include "SuperTextEdit.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loadBtn_clicked();

    void on_rescanBtn_clicked();

	void on_forceRead_clicked();

	void on_saveBtn_clicked();

    void on_saveasBtn_clicked();

	void on_jmpBtn_clicked();

private slots:
	void textBoxCursor_changed();

	void line_EOL_changed(int btnId, bool checked);
	void file_line_EOL_changed(int btnId, bool checked);

	void file_eol_decide(int val);


    void on_positionEdit_returnPressed();

    void on_btnDelEOL_clicked();

protected:
	void showHintsMessage( const QString& msg, int isError, int msec);
	void clearHighLight();

	void updateLineByteInfo(const QTextCursor& cursor, bool moveByteFlag, int moveByteIdx);
	void highLightCharacter(const CharacterInfo& chInfo);

	void forceMoveTextCursor(int pos);

	void pickEOLInfo( LineInfo* pLineInfo);
	E_EOL_TYPE getSavedOption();

	QString processHtmlText(const QString& content);
private:
    Ui::MainWindow *ui;

	QButtonGroup*  plineEOLGrp;
	QButtonGroup*  pfileEOLGrp;

	bool           m_isCursorMoveCauseChanged;
	bool           m_isReadFileCauseChanged;
	bool           m_isForceMoveTextCursor;
	bool           m_singleByteCursorMoveFlag;
	int            m_singleByteHighLightIdx;
};
#endif // MAINWINDOW_H
