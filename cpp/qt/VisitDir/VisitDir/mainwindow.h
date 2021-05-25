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
	void onVisitSomething(const QString &what,int type, int level);
	void onVisitDirDone();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
	// QThread*        m_runningThread;
	int             m_runningStatus;
};
#endif // MAINWINDOW_H
