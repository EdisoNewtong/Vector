#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QModelIndex>


#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QItemSelection>
#include <QPropertyAnimation>

#include "treenode.h"
#include "binarytreemodel.h"
#include "mysettingbtndelegate.h"
#include "mygraphicscircleitem.h"

namespace Ui {
class MainWindow;
}


class QStandardItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_newTreeBtn_clicked();

    void on_loadBtn_clicked();

    void on_saveBtn_clicked();

    void on_clearBtn_clicked();

    void on_saveGraphicBtn_clicked();

    void on_treeOptionBtn_clicked();

    void on_drawTreeBtn_clicked();

	void onSelectionNodeChanged(const QItemSelection &selected, const QItemSelection &deselected);
private:
    Ui::MainWindow *ui;

	QGraphicsScene* m_pScene;
	mygraphicscircleitem* m_pHighLightCircleFrame;
	QPropertyAnimation*   m_pHighLightAnimation;
	const int             m_animationMsec;
	const double          m_scale;

	// TreeView related object
	binarytreemodel* m_pTreeModel;
	mysettingbtndelegate*  m_btnDelegate;  // styled delegate

	// void drawTreeBySelectedItem( const QModelIndex& selected, int level); 
    QPair<QGraphicsEllipseItem*,QGraphicsSimpleTextItem*>  allocCircle(treenode* node);
};

#endif // MAINWINDOW_H