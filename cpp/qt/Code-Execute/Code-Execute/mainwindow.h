#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

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
    void onMenu0ItemTriggered(bool checked);
    void onMenu1ItemTriggered(bool checked);

    void onSaveToCfg(bool checked);
    void on_pushButton_clicked();

private:
    void initMenuBar();
    void initMenuBar0();
    void initMenuBar1();

    void initCfgFile();

    void saveToFile(bool needDelete);
    void highLightCatchedPosition(int startCursorPos);
    void de_highlight();
private:
    Ui::MainWindow *ui;
    QMenu*         m_pPrintSubMenu;
    QFile*         m_pCfgFile;
};
#endif // MAINWINDOW_H
