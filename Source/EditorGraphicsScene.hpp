#pragma once

#include <QGraphicsScene>
#include <QKeyEvent>
#include <map>

class ResizeableArrowItem;
class ResizeableRectItem;
class Model;
struct Camera;
class EditorTab;

class ItemPositionData final
{
public:
    struct RectPos final
    {
        qreal x = 0;
        qreal y = 0;
        QRectF rect;
        Camera* containingCamera = nullptr;

        bool operator == (const RectPos& rhs) const
        {
            return x == rhs.x && y == rhs.y && rect == rhs.rect && containingCamera == rhs.containingCamera;
        }
    };

    struct LinePos final
    {
        qreal x = 0;
        qreal y = 0;
        QLineF line;

        bool operator == (const LinePos& rhs) const
        {
            return x == rhs.x && y == rhs.y && line == rhs.line;
        }
    };

    void Save(QList<QGraphicsItem*>& items, Model& model, bool recalculateParentCamera);
    void Restore(Model& model);

    bool operator == (const ItemPositionData& rhs) const
    {
        if (mRects != rhs.mRects)
        {
            return false;
        }

        if (mLines != rhs.mLines)
        {
            return false;
        }

        return true;
    }

    bool operator != (const ItemPositionData& rhs) const
    {
        return !(*this == rhs);
    }

    size_t Count() const
    {
        return mRects.size() + mLines.size();
    }

    const RectPos* FirstRectPos() const
    {
        if (mRects.empty())
        {
            return nullptr;
        }
        return &mRects.begin()->second;
    }

    const LinePos* FirstLinePos() const
    {
        if (mLines.empty())
        {
            return nullptr;
        }
        return &mLines.begin()->second;
    }
private:
    void AddRect(ResizeableRectItem* pItem, Model& model, bool recalculateParentCamera);

    void AddLine(ResizeableArrowItem* pItem);

    std::map<ResizeableRectItem*, RectPos> mRects;
    std::map<ResizeableArrowItem*, LinePos> mLines;
};

class EditorGraphicsScene final : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit EditorGraphicsScene(EditorTab* pTab);
signals:
    void SelectionChanged(QList<QGraphicsItem*> oldItems, QList<QGraphicsItem*> newItems);
    void ItemsMoved(ItemPositionData oldPositions, ItemPositionData newPositions);
private:
    void mousePressEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent) override;

    void keyPressEvent(QKeyEvent* keyEvent) override;

    void CreateBackgroundBrush();
private:
    EditorTab* mTab = nullptr;
    QList<QGraphicsItem*> mOldSelection;
    ItemPositionData mOldPositions;
    bool mLeftButtonDown = false;
};
