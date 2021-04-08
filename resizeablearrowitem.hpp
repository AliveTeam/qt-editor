#pragma once

#include <QGraphicsLineItem>

class ICollision;

class ResizeableArrowItem final : public QGraphicsLineItem
{
public:
    ResizeableArrowItem(QGraphicsView* pView, ICollision* pLine);
    const static int kType = QGraphicsItem::UserType + 14;
    int type() const override { return kType; }
protected:
    void hoverLeaveEvent( QGraphicsSceneHoverEvent* aEvent ) override;
    void hoverMoveEvent( QGraphicsSceneHoverEvent* aEvent ) override;
    void mousePressEvent( QGraphicsSceneMouseEvent* aEvent ) override;
    void mouseMoveEvent( QGraphicsSceneMouseEvent* aEvent ) override;
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* aEvent ) override;
    void paint( QPainter* aPainter, const QStyleOptionGraphicsItem* aOption, QWidget* aWidget = nullptr ) override;
    QPainterPath shape() const override;
    QRectF boundingRect() const override;
private:
    void Init();
    void CalcWhichEndOfLineClicked( QPointF aPos, Qt::KeyboardModifiers aMods );
    void SetViewCursor(Qt::CursorShape cursor);
private:
    // For knowing which end to anchor line if required.
    enum eLinePoints
    {
        eLinePoints_None,
        eLinePoints_P1,
        eLinePoints_P2
    };
    eLinePoints m_endOfLineClicked = eLinePoints_None;
    QPointF m_fixedPoint;
    bool m_MouseIsDown = false;
    QGraphicsView* mView = nullptr;
    ICollision* mLine = nullptr;
};
