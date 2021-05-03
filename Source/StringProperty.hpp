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
    StringProperty(QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, std::string* pProperty);

    virtual QWidget* CreateEditorWidget(PropertyTreeWidget* pParent) override;

    virtual void Refresh() override;

    const void* GetPropertyLookUpKey() const override
    {
        return mProperty;
    }

private:
    std::string* mProperty = nullptr;
    QString mPrevValue;
    QUndoStack& mUndoStack;
};
