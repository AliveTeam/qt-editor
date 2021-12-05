#pragma once

#include <QGraphicsRectItem>
#include <QPixmap>

struct Camera;

class CameraGraphicsItem final  : public QGraphicsRectItem
{
public:
    CameraGraphicsItem(Camera* pCamera, int xpos, int ypos, int width, int height);
    void paint(QPainter* aPainter, const QStyleOptionGraphicsItem* aOption, QWidget* aWidget) override;
private:
    void LoadImages();

    Camera* mCamera = nullptr;
    struct Images final
    {
        QPixmap mCamera;
        QPixmap mForeground;
        QPixmap mBackground;
        QPixmap mWellForeground;
        QPixmap mWellBackground;
    };
    Images mImages;
};
