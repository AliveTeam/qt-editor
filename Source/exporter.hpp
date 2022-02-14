#ifndef EXPORTER_HPP
#define EXPORTER_HPP

#include <functional>
#include <QString>

void exportJsonToLvl(QString jsonPath, QString lvlPath, QString partialTemporaryFilePath, std::function<void(const QString)> onFailure);

#endif // EXPORTER_HPP
