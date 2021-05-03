#pragma once

#include "PropertyTreeItemBase.hpp"

struct BasicType;
class QUndoStack;

class BasicTypeProperty final : public PropertyTreeItemBase
{
public:
    BasicTypeProperty(QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, ObjectProperty* pProperty, BasicType* pBasicType);

    QWidget* CreateEditorWidget(PropertyTreeWidget* pParent) override;

    const void* GetPropertyLookUpKey() const override
    {
        return mProperty;
    }

private:
    QUndoStack& mUndoStack;
    ObjectProperty* mProperty = nullptr;
    BasicType* mBasicType = nullptr;
};
