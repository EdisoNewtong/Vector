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
    enum E_HEADER_TAG {
        E_ENABLE_STATE = 0,
        E_TAG_NAME,
        E_ORIGINAL_CHAR,
        E_VARIANT_CHAR_LIST,
        E_SEP_CHAR,
        E_DETAIL_INFO,
    };
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loadFileBtn_clicked();

    void on_saveFileBtn_clicked();

    void on_sPrevBtn_triggered(QAction *arg1);

    void on_sNextBtn_triggered(QAction *arg1);

    void on_searchBtn_clicked();

    void on_replace1Btn_clicked();

    void on_replaceAllBtn_clicked();

    void on_newCfgBtn_clicked();

    void on_newRowBtn_clicked();

    void on_loadCfgBtn_clicked();

    void on_saveCfgBtn_clicked();

    void on_checkLogicBtn_clicked();

    void on_actionShow_InfoBox_toggled(bool arg1);

private:
    void initUI();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
