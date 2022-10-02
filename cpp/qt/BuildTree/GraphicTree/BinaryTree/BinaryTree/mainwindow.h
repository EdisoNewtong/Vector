#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

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

private:
    Ui::MainWindow *ui;

	QGraphicsScene* m_pScene;
};

#endif // MAINWINDOW_H