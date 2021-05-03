#pragma once

#include "PropertyTreeItemBase.hpp"

struct Enum;
class QUndoStack;

class EnumProperty final : public PropertyTreeItemBase
{
public:
    EnumProperty(QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, QString propertyValue, Enum* pEnum);

    QWidget* CreateEditorWidget(PropertyTreeWidget* pParent) override;

private:
    QString mValue;
    QUndoStack& mUndoStack;
    Enum* mEnum = nullptr;
};
