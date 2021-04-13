#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsView>

struct MapObject;

class ResizeableRectItem final : public QGraphicsRectItem
{
public:
    ResizeableRectItem(QGraphicsView* pView,  MapObject* pMapObject );
    enum { Type = UserType + 1 };
    int type() const override { return Type; }
    QRectF SaveRect() const;
    void RestoreRect(const QRectF& rect);
    MapObject* GetMapObject() const { return mMapObject; }
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
private:
    eResize m_ResizeMode = eResize_None;
    static const quint32 kMinRectSize;
    QPixmap m_Pixmap;
    QGraphicsView* mView = nullptr;
    MapObject* mMapObject = nullptr;
};
