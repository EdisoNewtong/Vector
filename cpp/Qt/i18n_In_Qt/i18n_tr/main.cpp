#include <QApplication>
#include <QPushButton>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // the translation statement must ahead of QPushButton
    // Because tr(...) depends on the configration translation file
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    QTranslator translator;
    translator.load("hellotr_la");
    app.installTranslator(&translator);

    // tr is a short form of   "translation"
    // QPushButton helloBtn( QObject::tr("Hello world") );
    QPushButton helloBtn( QPushButton::tr("Hello world!") );
    helloBtn.resize(600,360);
    helloBtn.show();

    return app.exec();
}

