#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

struct lua_State;

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
    void on_runBtn_clicked();
private:
    void initLuaCode();
    void initLuaEnv();

private:
    Ui::MainWindow *ui;
    lua_State*     m_pLuaState;
};
#endif // MAINWINDOW_H
