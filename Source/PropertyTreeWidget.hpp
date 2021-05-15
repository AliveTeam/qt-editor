#pragma once

#include <QTreeWidget>
#include "model.hpp"

class PropertyTreeItemBase;
struct MapObject;
class CollisionObject;
class QGraphicsItem;
class QUndoStack;


inline const QString kIndent("    ");

class PropertyTreeWidget : public QTreeWidget
{
public:
    using QTreeWidget::QTreeWidget;

    PropertyTreeItemBase* FindObjectPropertyByKey(const void* pKey);

    void Populate(Model& model, QUndoStack& undoStack, QGraphicsItem* pItem);

    void Init();

private:
    void AddProperties(Model& model, QUndoStack& undoStack, QList<QTreeWidgetItem*>& items, std::vector<UP_ObjectProperty>& props, QGraphicsItem* pGraphicsItem);

};
