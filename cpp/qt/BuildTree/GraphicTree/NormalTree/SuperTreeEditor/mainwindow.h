#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGlobal>

class TreeModel;
class QGraphicsScene;

class myRectWithTextItem;
class TreeNode;

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
    void on_loadTreeFromFileBtn_clicked();

    void on_saveTreeToFileBtn_clicked();

    void on_clearTreeBtn_clicked();

    void on_drawTreeBtn_clicked();

    void on_saveAsPngBtn_clicked();

    void on_checkBoxOption0_stateChanged(int arg1);

    // Manually  Added by Edison
    void on_forceSetCheckBoxState(int checkedState);

    void on_testOnlyButton_clicked();

private:
    Ui::MainWindow *ui;

    TreeModel*    m_myTreeModel;
    QGraphicsScene*  m_pScene;

    myRectWithTextItem* m_pRectText1;
    qreal              m_maxYPos;

    qreal generateRenderObject(TreeNode* parentNode,myRectWithTextItem* parentRenderObject, int level, qreal previousX);


};

#endif // MAINWINDOW_H
