#include "EditorMainWindow.hpp"
#include <QApplication>
#include <QFile>
#include <QTranslator>
#include <QDebug>
#include "Exporter.hpp"
#include <functional>
#include <QtCore/qcommandlineparser.h>

void DoMapSizeTests();

int exportJsonToLvl(const QStringList& args);

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

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file."));
    parser.addPositionalArgument("dest", QCoreApplication::translate("main", "Destination file."));

    QCommandLineOption exportJsonToLvlOption("export", QCoreApplication::translate("main", "Export the .json file to the .lvl file. Usage: --export source dest"));
    parser.addOption(exportJsonToLvlOption);

    parser.process(app);

    const QStringList args = parser.positionalArguments();

    if (parser.isSet(exportJsonToLvlOption))
    {
        return exportJsonToLvl(args);
    }

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

int exportJsonToLvl(const QStringList& args)
{
    if (args.size() != 2)
    {
        std::cerr << "Incorrect usage of the --export option, should be --export source dest" << std::endl;
        return 1;
    }
    const QString source = args.at(0);
    const QString destination = args.at(1);
    int runResult = 0;
    exportJsonToLvl(source, destination, "relive_export", [&runResult](const QString& text) mutable
        {
            std::cerr << "Exporting failed. " << text << std::endl;
            runResult = 1;
        });
    return runResult;
}
