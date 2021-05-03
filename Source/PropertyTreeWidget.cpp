#include "PropertyTreeWidget.hpp"
#include "PropertyTreeItemBase.hpp"
#include "model.hpp"
#include "resizeablearrowitem.hpp"
#include "resizeablerectitem.hpp"
#include "StringProperty.hpp"
#include "BasicTypeProperty.hpp"
#include "EnumProperty.hpp"
#include <QHeaderView>

PropertyTreeItemBase* PropertyTreeWidget::FindStringProperty(std::string* pPropToFind)
{
    if (mMapObject)
    {
        if (&mMapObject->mName == pPropToFind)
        {
            for (int i = 0; i < topLevelItemCount(); i++)
            {
                auto pItem = static_cast<PropertyTreeItemBase*>(topLevelItem(i));
                if (pItem->GetMapObject() == mMapObject)
                {
                    return pItem;
                }
            }
        }
    }

    if (mCollision)
    {
        // TODO
    }

    return nullptr;
}

void PropertyTreeWidget::SetMapObject(MapObject* pMapObject)
{
    mMapObject = pMapObject;
    mCollision = nullptr;
}

void PropertyTreeWidget::SetCollisionObject(CollisionObject* pCollision)
{
    mMapObject = nullptr;
    mCollision = pCollision;
}

static const QString kIndent("    ");

void PropertyTreeWidget::Populate(Model& model, QUndoStack& undoStack, QGraphicsItem* pItem)
{
    auto pLine = qgraphicsitem_cast<ResizeableArrowItem*>(pItem);
    auto pRect = qgraphicsitem_cast<ResizeableRectItem*>(pItem);

    QList<QTreeWidgetItem*> items;
    QTreeWidgetItem* parent = nullptr;
    if (pRect)
    {
        MapObject* pMapObject = pRect->GetMapObject();
        SetMapObject(pMapObject);

        items.append(new StringProperty(pMapObject, undoStack, parent, kIndent + "Name", &pMapObject->mName));
        AddProperties(model, undoStack, items, pMapObject->mProperties);
    }
    else if (pLine)
    {
        CollisionObject* pCollisionItem = pLine->GetCollisionItem();
        SetCollisionObject(pCollisionItem);
        AddProperties(model, undoStack, items, pCollisionItem->mProperties);
    }

#ifdef _WIN32
    for (int i = 0; i < items.count(); i++)
    {
        const int b = (i % 2) == 0 ? 191 : 222;
        items[i]->setBackground(0, QColor(255, 255, b));
        items[i]->setBackground(1, QColor(255, 255, b));

    }
#endif

    insertTopLevelItems(0, items);
}

void PropertyTreeWidget::Init()
{
    // Two columns, property and value
    setColumnCount(2);

    // Set the header text
    QStringList headerStrings;
    headerStrings.append("Property");
    headerStrings.append("Value");
    setHeaderLabels(headerStrings);

    setAlternatingRowColors(true);
    setStyleSheet("QTreeView::item { height:23px; font:6px; padding:0px; margin:0px; }");

    header()->resizeSection(0, 200);
    header()->resizeSection(1, 90);

    setUniformRowHeights(true);

    setRootIsDecorated(false);

    connect(this, &QTreeWidget::currentItemChanged, this, [&](QTreeWidgetItem* current, QTreeWidgetItem* prev)
        {
            if (prev)
            {
                setItemWidget(prev, 1, nullptr);
            }
        });

    connect(this, &QTreeWidget::itemClicked, this, [&](QTreeWidgetItem* item, int column)
        {
            if (column == 1)
            {
                setItemWidget(item, column, static_cast<PropertyTreeItemBase*>(item)->CreateEditorWidget(this));
            }
        });
}

void PropertyTreeWidget::AddProperties(Model& model, QUndoStack& undoStack, QList<QTreeWidgetItem*>& items, std::vector<UP_ObjectProperty>& props)
{
    QTreeWidgetItem* parent = nullptr;
    for (UP_ObjectProperty& property : props)
    {
        if (property->mVisible)
        {
            if (property->mType == ObjectProperty::Type::BasicType)
            {
                BasicType* pBasicType = model.FindBasicType(property->mTypeName);
                items.append(new BasicTypeProperty(undoStack, parent, kIndent + property->mName.c_str(), property->mBasicTypeValue, pBasicType));
            }
            else
            {
                Enum* pEnum = model.FindEnum(property->mTypeName);
                items.append(new EnumProperty(undoStack, parent, kIndent + property->mName.c_str(), property->mEnumValue.c_str(), pEnum));
            }
        }
    }
}