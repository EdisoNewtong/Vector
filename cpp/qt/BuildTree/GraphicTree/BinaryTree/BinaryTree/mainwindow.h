#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QModelIndex>
#include <QGraphicsEllipseItem>

#include "binarytreemodel.h"
#include "mysettingbtndelegate.h"

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

private:
    Ui::MainWindow *ui;

	QGraphicsScene* m_pScene;

	// TreeView related object
	binarytreemodel* m_pTreeModel;
	mysettingbtndelegate*  m_btnDelegate;  // styled delegate

	void drawTreeBySelectedItem( const QModelIndex& selected, int level); 
    QGraphicsEllipseItem* allocCircle(const qreal& r, const QString& text);
};

#endif // MAINWINDOW_H