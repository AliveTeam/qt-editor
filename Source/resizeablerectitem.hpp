#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsView>
#include "IGraphicsItem.hpp"

struct MapObject;
class ISyncPropertiesToTree;

class ResizeableRectItem final : public IGraphicsItem, public QGraphicsRectItem
{
public:
    ResizeableRectItem(QGraphicsView* pView, MapObject* pMapObject, ISyncPropertiesToTree& propSyncer);
    enum { Type = UserType + 1 };
    int type() const override { return Type; }
    QRectF SaveRect() const;
    void RestoreRect(const QRectF& rect);
    MapObject* GetMapObject() const { return mMapObject; }
 
    void SyncInternalObject() override
    {
        SyncToMapObject();
    }

    std::vector<UP_ObjectProperty>& GetProperties() override
    {
        return mMapObject->mProperties;
    }

protected:  // From QGraphicsItem
    void mousePressEvent(QGraphicsSceneMouseEvent* aEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* aEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* aEvent) override;
    void paint(QPainter* aPainter, const QStyleOptionGraphicsItem* aOption, QWidget* aWidget = nullptr) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* aEvent) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* aEvent) override;
    QVariant itemChange(GraphicsItemChange aChange, const QVariant& aValue) override;
private:
    enum eResize
    {
        eResize_None,
        eResize_TopLeftCorner,
        eResize_TopRightCorner,
        eResize_BottomLeftCorner,
        eResize_BottomRightCorner,
        eResize_Top,
        eResize_Left,
        eResize_Right,
        eResize_Bottom
    };
    void Init();
    eResize getResizeLocation( QPointF aPos, QRectF aRect );
    bool IsNear( qreal xP1, qreal xP2 );
    void onResize( QPointF aPos );
    void SetViewCursor(Qt::CursorShape cursor);
    qreal CalcZPos() const;
    void SyncToMapObject();
    void PosOrRectChanged();
private:
    eResize m_ResizeMode = eResize_None;
    static const quint32 kMinRectSize;
    QPixmap m_Pixmap;
    QGraphicsView* mView = nullptr;
    MapObject* mMapObject = nullptr;
    ISyncPropertiesToTree& mPropSyncer;
};
