#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tableitemmodel.h"

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

    void on_saveBtn_clicked();

    void on_clearBtn_clicked();

    void on_optionBtn_clicked();

    void on_picclrBtn_clicked();

    void on_tstBtn1_clicked();

    void on_tstBtn2_clicked();


    // void onStackCommandSelectionChanged(const QItemSelection& newSelection, const QItemSelection& oldSelection);

    void on_addBtn_clicked();

    void on_minusBtn_clicked();

private:
    Ui::MainWindow *ui;

	TableItemModel*     m_pStackModel;


    void showHintsMessage( const QString& msg, int tag, int msec);

};
#endif // MAINWINDOW_H
