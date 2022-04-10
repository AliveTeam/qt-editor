#pragma once

#include <functional>
#include <QString>

namespace ReliveAPI
{
    class Context;
}

bool exportJsonToLvl(QString jsonPath, QString lvlPath, QString partialTemporaryFilePath, std::function<void(const QString)> onFailure, ReliveAPI::Context& context);
