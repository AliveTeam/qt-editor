#include "EditorGraphicsScene.hpp"
#include <QPainter>

EditorGraphicsScene::EditorGraphicsScene()
{
    CreateBackgroundBrush();
}

void EditorGraphicsScene::CreateBackgroundBrush()
{
    int gridSizeX = 25;
    int gridSizeY = 20;

    // Paint a single grid background onto a pixmap.
    QImage singleGrid(gridSizeX, gridSizeY, QImage::Format_RGB32);
    singleGrid.fill(Qt::white);
    {
        QPainter painter(&singleGrid);

        // Lighter background
        painter.setPen(QPen(QColor(240, 240, 240)));

        // Draw grid mid lines
        qreal midx = gridSizeX / 2;
        qreal midy = gridSizeY / 2;
        painter.drawLine(0, midy, gridSizeX, midy);
        painter.drawLine(midx, 0, midx, gridSizeY);

        // Darker foreground
        painter.setPen(QPen(QColor(180, 180, 180)));

        // Draw main grid lines
        painter.drawRect(0, 0, gridSizeX, gridSizeY);

        painter.end();
    }

    // Use this as the background brush which will be tiled and scale
    // properly when the scene is zoomed. And also prevents rendering issues compared to
    // the overriding drawBackground() method.
    QBrush brushBackground(singleGrid);
    setBackgroundBrush(brushBackground);
}

void EditorGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* pEvent)
{
    mOldSelection = selectedItems();
    QGraphicsScene::mousePressEvent(pEvent);
}

void EditorGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent)
{
    QGraphicsScene::mouseMoveEvent(pEvent);
}

void EditorGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent)
{
    QGraphicsScene::mouseReleaseEvent(pEvent);
    emit SelectionChanged(mOldSelection, selectedItems());
}
