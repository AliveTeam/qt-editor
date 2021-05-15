#pragma once

#include <QTreeWidgetItem>
#include "PropertyTreeWidget.hpp"
#include "bigspinbox.hpp"

struct LinkedProperty
{
    LinkedProperty(PropertyTreeWidget* pTreeWidget, ObjectProperty* pProperty, QGraphicsItem* pGraphicsItem)
        : mTreeWidget(pTreeWidget), mProperty(pProperty), mGraphicsItem(pGraphicsItem)
    {

    }
    PropertyTreeWidget* mTreeWidget = nullptr;
    ObjectProperty* mProperty = nullptr;
    QGraphicsItem* mGraphicsItem = nullptr;
};

class PropertyTreeItemBase : public QTreeWidgetItem
{
public:
    using QTreeWidgetItem::QTreeWidgetItem;

    virtual QWidget* CreateEditorWidget(PropertyTreeWidget* pParent) = 0;

    virtual void Refresh() = 0;

    virtual const void* GetPropertyLookUpKey() const = 0;
};
