#pragma once

#include <QGraphicsRectItem>

struct Camera;

class CameraGraphicsItem : public QGraphicsRectItem
{
public:
    CameraGraphicsItem(Camera* pCamera, int xpos, int ypos, int width, int height);
    void paint(QPainter* aPainter, const QStyleOptionGraphicsItem* aOption, QWidget* aWidget) override;
private:
    Camera* mCamera = nullptr;
};
