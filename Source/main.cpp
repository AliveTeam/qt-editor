#include "editormainwindow.hpp"
#include <QApplication>
#include <QFile>

void DoMapSizeTests();

int main(int argc, char *argv[])
{
    DoMapSizeTests();

    QApplication a(argc, argv);
    EditorMainWindow w;

    QFile File(":/stylesheets/rsc/stylesheets/dark-stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(File.readAll());

    a.setStyleSheet(styleSheet);
    a.setWindowIcon(QIcon(":/icons/rsc/icons/icon.png"));
    w.setWindowIcon(QIcon(":/icons/rsc/icons/icon.png"));

    w.show();
    return a.exec();
}
