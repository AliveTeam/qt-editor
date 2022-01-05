#include "editormainwindow.hpp"
#include <QApplication>
#include <QFile>
#include <QTranslator>
#include <QDebug>

void DoMapSizeTests();

int main(int argc, char *argv[])
{
    DoMapSizeTests();

    QTranslator translator;

    if (!translator.load("qt-editor_German"))
    {
        qDebug() << "Translator load failed";
    }

    QApplication app(argc, argv);
    app.installTranslator(&translator);

    EditorMainWindow w;

    QFile File(":/stylesheets/rsc/stylesheets/dark-stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(File.readAll());

    app.setStyleSheet(styleSheet);
    app.setWindowIcon(QIcon(":/icons/rsc/icons/icon.png"));
    w.setWindowIcon(QIcon(":/icons/rsc/icons/icon.png"));

    w.show();
    return app.exec();
}
