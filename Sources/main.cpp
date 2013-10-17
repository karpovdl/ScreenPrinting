#include <QtGui/QApplication>
#include <QtGui>
#include <QtCore>

#include "mainwindow.h"
#include "smallscreenwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Screen printing");
    app.setApplicationVersion("0.0.0.1");
    app.setOrganizationName("NIIIS");
    app.setQuitOnLastWindowClosed(true);
    app.setStyle("cleanlooks");// Установка стиля форм

    // Русский формат для класса строки
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1251"));

    // Установка путей
    QStringList list_path;
    QDir dirPlugin = QDir(qApp->applicationDirPath()+"/plugins//");
    list_path << dirPlugin.absolutePath() << qApp->libraryPaths();
    qApp->setLibraryPaths(list_path);

    QStringList args = app.arguments();

    if(args.size() > 1 && args.at(1) == "/sw") {
        SmallScreenWindow small_screen_window;
        small_screen_window.show();

        return app.exec();
    }

    MainWindow main_window;
    main_window.show();

    return app.exec();
}

