#pragma once

#include <QGraphicsScene>

class EditorGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    EditorGraphicsScene();
signals:
    void SelectionChanged(QList<QGraphicsItem*> oldItems, QList<QGraphicsItem*> newItems);
private:
    void mousePressEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void CreateBackgroundBrush();
private:
    QList<QGraphicsItem*> mOldSelection;
};
