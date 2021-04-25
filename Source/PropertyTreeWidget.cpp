#include "PropertyTreeWidget.hpp"
#include "PropertyTreeItemBase.hpp"
#include "model.hpp"

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
