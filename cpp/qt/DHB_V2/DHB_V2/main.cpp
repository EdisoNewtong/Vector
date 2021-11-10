#include "myhardcodewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    myhardcodewindow mainWin;
    mainWin.initUI();
    mainWin.show();

    return a.exec();
}
