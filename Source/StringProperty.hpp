#pragma once

#include <QUndoCommand>
#include <QLineEdit>
#include <QUndoStack>
#include "PropertyTreeItemBase.hpp"

class PropertyTreeWidget;

class ChangeStringPropertyCommand : public QUndoCommand
{
public:
    ChangeStringPropertyCommand(PropertyTreeWidget* pTreeWidget, std::string* pProperty, QString propertyName, QString oldValue, QString newValue);

    void undo() override;

    void redo() override;

private:
    PropertyTreeWidget* mTreeWidget = nullptr;
    std::string* mProperty = nullptr;
    QString mOldValue;
    QString mNewValue;
};

class StringProperty : public QObject, public PropertyTreeItemBase
{
    Q_OBJECT
public:
    StringProperty(MapObject* pMapObject, QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, std::string* pProperty);

    virtual QWidget* CreateEditorWidget(PropertyTreeWidget* pParent) override;

    virtual void Refresh() override;

    virtual MapObject* GetMapObject() override
    {
        return mMapObject;
    }

private:
    MapObject* mMapObject = nullptr;
    std::string* mProperty = nullptr;
    QString mPrevValue;
    QUndoStack& mUndoStack;
};


struct Enum;

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

struct BasicType;

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
