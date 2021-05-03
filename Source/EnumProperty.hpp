#pragma once

#include "PropertyTreeItemBase.hpp"
#include <QUndoCommand>

struct Enum;
class QUndoStack;

class ChangeEnumPropertyCommand : public QUndoCommand
{
public:
    ChangeEnumPropertyCommand(PropertyTreeWidget* pTreeWidget, ObjectProperty* pProperty, Enum* pEnum, int oldIdx, int newIdx);

    void undo() override;

    void redo() override;

private:
    PropertyTreeWidget* mTreeWidget = nullptr;
    ObjectProperty* mProperty = nullptr;
    Enum* mEnum = nullptr;
    int mOldIdx = 0;
    int mNewIdx = 0;
};

class EnumProperty final : public QObject, public PropertyTreeItemBase
{
    Q_OBJECT
public:
    EnumProperty(QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, ObjectProperty* pProperty, Enum* pEnum);

    QWidget* CreateEditorWidget(PropertyTreeWidget* pParent) override;

    void Refresh() override;

    const void* GetPropertyLookUpKey() const override
    {
        return mProperty;
    }

private:
    void GetPropertyIndex();

    QUndoStack& mUndoStack;
    ObjectProperty* mProperty = nullptr;
    Enum* mEnum = nullptr;
    int mOldIdx = -1;
};
