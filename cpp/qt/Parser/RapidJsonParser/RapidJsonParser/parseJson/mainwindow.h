#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStack>
#include <QStandardItemModel>

#include "rapidjson/document.h"



namespace Ui {
class MainWindow;
}

class QItemSelection;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
	enum Visit_Step
	{
		E_NONE,
		// Common
		E_VISIT_NEXT,
		// Object
		E_VISIT_OBJECT_KEY,
		E_VISIT_OBJECT_VALUE,
		E_VISIT_OBJECT_ALL,
		// Array
		E_VISIT_ARRAY_ELEMENT,
		E_VISIT_ARRAY_ALL
	};
private:
	struct VisitInfo
	{
		Visit_Step tag;

		rapidjson::Value::MemberIterator member_iter;
		rapidjson::Value::ValueIterator  element_iter;

		rapidjson::Value* visitValue;
	};

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

	void onTreeItemSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);



private:
    Ui::MainWindow *ui;

	rapidjson::Value*     m_visitValue;
	rapidjson::Document*  m_pDoc;
	rapidjson::Value::MemberIterator m_it;	// Object's member iterator
	rapidjson::Value::ValueIterator  m_eleIt;	// Array's element iterator
	QStack<VisitInfo> m_visitStack;



	bool m_isParseOK;
	Visit_Step m_visitTag;
	int  m_visitStep;

	void clearDoc();
	void do_Parse(const char* content);

    void printValueDetail(rapidjson::Value* jsonValue,int nStep);
    void highLightText(int startpos,int endpos);


	// For Tree Show
	QStandardItemModel* m_pTreeModel;
	void clearTree();
	void setupTreeModel();
	void fillDataIntoModel(QStandardItem* parent, rapidjson::Value* jsonValue,int tag);

};

#endif // MAINWINDOW_H
