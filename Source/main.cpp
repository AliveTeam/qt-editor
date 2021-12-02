#include "editormainwindow.hpp"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EditorMainWindow w;

    QFile File(":/stylesheets/rsc/stylesheets/dark-stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(File.readAll());

    w.setStyleSheet(styleSheet);
    w.setWindowIcon(QIcon(":/icons/rsc/icons/icon.png"));

    w.show();
    return a.exec();
}
