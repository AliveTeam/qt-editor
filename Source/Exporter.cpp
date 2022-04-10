#include "Exporter.hpp"
#include <QFileDialog>
#include <QUuid>
#include "ReliveApiWrapper.hpp"
#include "file_api.hpp"

bool exportJsonToLvl(QString jsonPath, QString lvlPath, QString partialTemporaryFilePath, std::function<void(const QString)> onFailure, ReliveAPI::Context& context)
{
    ReliveAPI::FileIO fileIo;
    auto fnExport = [&]()
    {
        std::vector<std::string> resourceSources; // TODO: Wire into UI

        QUuid uuid = QUuid::createUuid();
        QString tempFileFullPath = QDir::toNativeSeparators(
            QDir::tempPath() + "/" +
            partialTemporaryFilePath +
            "_" +
            uuid.toString(QUuid::WithoutBraces) + ".lvl.tmp");

        // Export to a temp lvl file
        ReliveAPI::ImportPathJsonToBinary(
            fileIo,
            jsonPath.toStdString(),
            lvlPath.toStdString(),
            tempFileFullPath.toStdString(),
            resourceSources,
            context);

        // Then overwrite the original lvl with the temp one
        if (!QFile::remove(lvlPath))
        {
            onFailure("Failed to delete " + lvlPath + " in order to replace it with the updated lvl");
            QFile::remove(tempFileFullPath);
            return false;
        }

        if (!QFile::rename(tempFileFullPath, lvlPath))
        {
            onFailure("Failed to rename from " + tempFileFullPath + " to " + lvlPath);
            return false;
        }

        return true;
    };

    return ExecApiCall(fnExport, onFailure);
}
