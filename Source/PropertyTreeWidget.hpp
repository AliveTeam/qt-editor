#pragma once

#include <QTreeWidget>

class PropertyTreeItemBase;
struct MapObject;
class CollisionObject;

class PropertyTreeWidget : public QTreeWidget
{
public:
    using QTreeWidget::QTreeWidget;

    PropertyTreeItemBase* FindStringProperty(std::string* pPropToFind);

    void SetMapObject( MapObject* pMapObject );

    void SetCollisionObject(CollisionObject* pCollision);

private:
    MapObject* mMapObject = nullptr;
    CollisionObject* mCollision = nullptr;
};
