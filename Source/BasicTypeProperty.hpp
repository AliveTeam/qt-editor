#pragma once

#include "PropertyTreeItemBase.hpp"
#include <QUndoCommand>

struct BasicType;
class QUndoStack;
class BigSpinBox;

class ChangeBasicTypePropertyCommand : public QUndoCommand
{
public:
    ChangeBasicTypePropertyCommand(PropertyTreeWidget* pTreeWidget, ObjectProperty* pProperty, BasicType* pBasicType, int oldValue, int newValue);

    void undo() override;

    void redo() override;

    int id() const override
    {
        return 1;
    }

    bool mergeWith(const QUndoCommand* command) override;

private:
    void UpdateText();

    PropertyTreeWidget* mTreeWidget = nullptr;
    ObjectProperty* mProperty = nullptr;
    BasicType* mBasicType = nullptr;
    int mOldValue = 0;
    int mNewValue = 0;
    qint64 mTimeStamp = 0;
};

class BasicTypeProperty final : public QObject, public PropertyTreeItemBase
{
    Q_OBJECT
public:
    BasicTypeProperty(QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, ObjectProperty* pProperty, BasicType* pBasicType);

    QWidget* CreateEditorWidget(PropertyTreeWidget* pParent) override;

    const void* GetPropertyLookUpKey() const override
    {
        return mProperty;
    }

    void Refresh() override;

private:
    QUndoStack& mUndoStack;
    ObjectProperty* mProperty = nullptr;
    BasicType* mBasicType = nullptr;
    int mOldValue = 0;
    BigSpinBox* mSpinBox = nullptr;
};
