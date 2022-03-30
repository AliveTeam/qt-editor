#pragma once

#include <functional>
#include <QString>

bool exportJsonToLvl(QString jsonPath, QString lvlPath, QString partialTemporaryFilePath, std::function<void(const QString)> onFailure);
