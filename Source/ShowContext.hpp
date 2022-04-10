#pragma once

#include "ReliveApiWrapper.hpp"
#include <QMessageBox>

inline void ShowContext(const ReliveAPI::Context& context)
{
    // TODO: Should be a dialog showing the source file of each warning
    QString fatMessage;
    for (const auto& remapped : context.RemappedEnumValues())
    {
        QString tmp;
        tmp = "Enum value " + QString(remapped.mEnumValueInJson.c_str()) + QString(" remapped to ") + QString(remapped.mValueUsed.c_str()) + QString(" for enum type ") + QString(remapped.mEnumTypeName.c_str()) + QString("\n");
        fatMessage += tmp;
    }

    for (const auto& missingProperty : context.MissingJsonProperties())
    {
        QString tmp;
        tmp = QString("Property ") + QString(missingProperty.mPropertyName.c_str()) + QString(" was not found in type ") + QString(missingProperty.mStructureTypeName.c_str()) + QString("\n");
        fatMessage += tmp;
    }

    QMessageBox::warning(nullptr, "Context warnings", fatMessage);

}
