#include "mainwindow.h"
#include <QApplication>

// #define SIGNLE_APP_FLAG

#ifdef  SIGNLE_APP_FLAG
  #include <QSharedMemory>
  #include <QMessageBox>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#ifdef SIGNLE_APP_FLAG
    // 使用唯一的键（建议使用组织名+应用名）
    const char* MEM_KEY = "com.vector.DHB";
    QSharedMemory sharedMemory(MEM_KEY);

    // 尝试创建共享内存，失败说明已有实例
    if ( !sharedMemory.create(1)) {
        if ( sharedMemory.error() == QSharedMemory::AlreadyExists) {
            QMessageBox::warning(nullptr, "警告", "应用程序已在运行！");
            return 1;
        }
        // 处理其他错误
    }
#endif

    MainWindow window;
    window.show();

    return app.exec();
}
