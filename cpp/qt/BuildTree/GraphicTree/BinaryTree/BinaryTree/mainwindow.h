#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QModelIndex>
#include <QVector>


#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QItemSelection>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QPointF>

#include "treenode.h"
#include "binarytreemodel.h"
#include "mysettingbtndelegate.h"
#include "mygraphicscircleitem.h"
#include "mygraphicsrectitem.h"
#include "mytextanimationitem.h"

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





    void on_preOrderBtn_clicked();

    void on_inOrderBtn_clicked();

    void on_postOrderBtn_clicked();
    void on_stopAnimationBtn_clicked();

    void on_applyOptionBtn_clicked();

    void on_genCodeBtn_clicked();

private:
    Ui::MainWindow *ui;

	QGraphicsScene* m_pScene;
	mygraphicscircleitem* m_pHighLightCircleFrame;
	QPropertyAnimation*   m_pHighLightAnimation;

	QPropertyAnimation*        m_pEnumerateHVBarAni;
	mygraphicsrectitem*        m_pEnumerateMoveRect;
	QPropertyAnimation*        m_pEnumerateMoveRectAni;

	QPropertyAnimation*        m_pEnumerateStatusTextAni;
	MyTextAnimationItem*       m_pEnumerateTextItem;

	QParallelAnimationGroup*   m_pEnumerateAnimationGrp;

	const int             m_animationMsec;
	const double          m_scale;

	bool                  m_bEnumerateNodeWithAnimation;

	// TreeView related object
	binarytreemodel* m_pTreeModel;
	mysettingbtndelegate*  m_btnDelegate;  // styled delegate
    treenode* m_lastRenderedTreeRootNode;

	// void drawTreeBySelectedItem( const QModelIndex& selected, int level); 
    QPair<QGraphicsEllipseItem*,QGraphicsSimpleTextItem*>  allocCircle(treenode* node);
	treenode* getSelectedNode();
	QPoint getHVScrollBarValueForMakeCircleCenter(QGraphicsEllipseItem* pCircle, bool* pIsOK);


	void enumerateNodes(int type, const QVector<treenode*>& nodeList, bool bAnimationFlag);
	void stopEnuerateAnimationIfNecessary(bool needForce = false);

    void reRenderPreRootTree();
	
};

#endif // MAINWINDOW_H
