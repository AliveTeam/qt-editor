#pragma once

#include <QTreeWidget>
#include "model.hpp"

class PropertyTreeItemBase;
struct MapObject;
class CollisionObject;
class QGraphicsItem;
class QUndoStack;

class PropertyTreeWidget : public QTreeWidget
{
public:
    using QTreeWidget::QTreeWidget;

    PropertyTreeItemBase* FindStringProperty(std::string* pPropToFind);

    void SetMapObject( MapObject* pMapObject );

    void SetCollisionObject(CollisionObject* pCollision);

    void Populate(Model& model, QUndoStack& undoStack, QGraphicsItem* pItem);

    void Init();

private:
    void AddProperties(Model& model, QUndoStack& undoStack, QList<QTreeWidgetItem*>& items, std::vector<UP_ObjectProperty>& props);

    MapObject* mMapObject = nullptr;
    CollisionObject* mCollision = nullptr;
};
