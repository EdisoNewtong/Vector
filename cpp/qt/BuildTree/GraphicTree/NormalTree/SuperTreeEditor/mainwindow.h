#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
