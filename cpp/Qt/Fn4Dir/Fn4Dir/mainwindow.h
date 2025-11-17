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
    void on_encBtn_clicked();

    void on_decBtn_clicked();

    void on_encWithArgsBtn_clicked();

    void on_cmp2FilesBtn_clicked();

private:
    void encode1File_TestCase(const QString& filePath);
    void decode1File_TestCase(const QString& filePath);

    void encode_a_GivenFile(const QString& filePath);
    void decode_a_GivenFile(const QString& filePath);

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
