#pragma once

#include <QTreeWidgetItem>
#include "PropertyTreeWidget.hpp"
#include "bigspinbox.hpp"

class PropertyTreeItemBase : public QTreeWidgetItem
{
public:
    using QTreeWidgetItem::QTreeWidgetItem;

    virtual QWidget* CreateEditorWidget(PropertyTreeWidget* pParent)
    {
        auto spin = new BigSpinBox(pParent);
        return spin;
    }

    virtual void Refresh()
    {

    }

    virtual MapObject* GetMapObject()
    {
        return nullptr;
    }
};
