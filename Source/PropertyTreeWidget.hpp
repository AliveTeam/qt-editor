#pragma once

#include <QTreeWidget>

class PropertyTreeItemBase;
struct MapObject;
class ICollision;

class PropertyTreeWidget : public QTreeWidget
{
public:
    using QTreeWidget::QTreeWidget;

    PropertyTreeItemBase* FindStringProperty(std::string* pPropToFind);

    void SetMapObject( MapObject* pMapObject );

    void SetCollisionObject(ICollision* pCollision);

private:
    MapObject* mMapObject = nullptr;
    ICollision* mCollision = nullptr;
};
