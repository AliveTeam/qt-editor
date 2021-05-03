#pragma once

#include "PropertyTreeItemBase.hpp"

struct BasicType;
class QUndoStack;

class BasicTypeProperty final : public PropertyTreeItemBase
{
public:
    BasicTypeProperty(QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, int propertyValue, BasicType* pBasicType);

    QWidget* CreateEditorWidget(PropertyTreeWidget* pParent) override;

private:
    int mValue = 0;
    QUndoStack& mUndoStack;
    BasicType* mBasicType = nullptr;
};
