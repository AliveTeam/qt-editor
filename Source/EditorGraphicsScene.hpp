#pragma once

#include <QGraphicsScene>
#include <map>

class ResizeableArrowItem;
class ResizeableRectItem;

class ItemPositionData
{
public:
    struct RectPos
    {
        qreal x = 0;
        qreal y = 0;
        QRectF rect;

        bool operator == (const RectPos& rhs) const
        {
            return x == rhs.x && y == rhs.y && rect == rhs.rect;
        }
    };

    struct LinePos
    {
        qreal x = 0;
        qreal y = 0;
        QLineF line;

        bool operator == (const LinePos& rhs) const
        {
            return x == rhs.x && y == rhs.y && line == rhs.line;
        }
    };

    void Save(QList<QGraphicsItem*>& items);
    void Restore();

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
    void AddRect(ResizeableRectItem* pItem);

    void AddLine(ResizeableArrowItem* pItem);

    std::map<ResizeableRectItem*, RectPos> mRects;
    std::map<ResizeableArrowItem*, LinePos> mLines;
};

class EditorGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    EditorGraphicsScene();
signals:
    void SelectionChanged(QList<QGraphicsItem*> oldItems, QList<QGraphicsItem*> newItems);
    void ItemsMoved(ItemPositionData oldPositions, ItemPositionData newPositions);
private:
    void mousePressEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void CreateBackgroundBrush();
private:
    QList<QGraphicsItem*> mOldSelection;
    ItemPositionData mOldPositions;
};
