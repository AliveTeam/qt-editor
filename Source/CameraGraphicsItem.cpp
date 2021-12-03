#include "CameraGraphicsItem.hpp"
#include <QPen>
#include <QPainter>
#include "model.hpp"

CameraGraphicsItem::CameraGraphicsItem(Camera* pCamera, int xpos, int ypos, int width, int height) : QGraphicsRectItem(xpos, ypos, width, height), mCamera(pCamera)
{
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor::fromRgb(120, 120, 120));
    setPen(pen);

    setZValue(1.0);
}

void CameraGraphicsItem::paint(QPainter* aPainter, const QStyleOptionGraphicsItem* aOption, QWidget* aWidget)
{
    QGraphicsRectItem::paint(aPainter, aOption, aWidget);
    QRectF bounds = boundingRect();

    if (mCamera && !mCamera->mName.empty())
    {
        aPainter->setBrush(QBrush(QColor::fromRgb(240, 240, 240)));
        aPainter->setPen(Qt::black);
        aPainter->setOpacity(0.8);

        const QRect textRect = QRect(rect().x() + 10, rect().y() + 10, 80, 20);
        aPainter->drawRect(textRect);
        aPainter->drawText(textRect, Qt::AlignCenter, mCamera->mName.c_str());
    }
}
