#include "EditorGraphicsScene.hpp"
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include "resizeablearrowitem.hpp"
#include "resizeablerectitem.hpp"

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
    if (pEvent->button() != Qt::LeftButton)
    {
        qDebug() << "Ignore non left click";
        pEvent->ignore();
        return;
    }

    if (pEvent->button() == Qt::LeftButton)
    {
        mLeftButtonDown = true;

        qDebug() << "left press";

        // Record what we had before a click
        mOldSelection = selectedItems();

        // Do the click
        QGraphicsScene::mousePressEvent(pEvent);

        if (mOldSelection != selectedItems())
        {
            qDebug() << "left press selection changed";

            // A single item just got selected by being clicked on
            emit SelectionChanged(mOldSelection, selectedItems());
            mOldSelection = selectedItems();
        }

        // Save the locations of what is selected after click
        mOldPositions.Save(mOldSelection);
    }
    else
    {
        qDebug() << "mouse press";
        QGraphicsScene::mousePressEvent(pEvent);
    }
}

void EditorGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent)
{
    QGraphicsScene::mouseMoveEvent(pEvent);
}

void EditorGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent)
{
    // Handle the button up
    QGraphicsScene::mouseReleaseEvent(pEvent);

    if (pEvent->button() == Qt::LeftButton)
    {
        qDebug() << "left release";

        // Find out what is selected
        QList<QGraphicsItem*> currentSelection = selectedItems();

        if (mOldSelection != currentSelection)
        {
            // Between mouse up/down the selection changed
            qDebug() << "left release selection changed";
            emit SelectionChanged(mOldSelection, currentSelection);
        }

        if (mOldPositions.Count() > 0)
        {
            // Get the position of where the selected items are now
            ItemPositionData newPositions;
            newPositions.Save(currentSelection);

            if (mOldPositions != newPositions)
            {
                qDebug() << "left release positions changed";

                // They've moved since mouse down
                emit ItemsMoved(mOldPositions, newPositions);
            }
        }

        mLeftButtonDown = false;
    }
}

void ItemPositionData::Save(QList<QGraphicsItem*>& items)
{
    mRects.clear();
    mLines.clear();

    for (auto& item : items)
    {
        ResizeableRectItem* pRect = qgraphicsitem_cast<ResizeableRectItem*>(item);
        if (pRect)
        {
            AddRect(pRect);
        }
        else
        {
            ResizeableArrowItem* pArrow = qgraphicsitem_cast<ResizeableArrowItem*>(item);
            if (pArrow)
            {
                AddLine(pArrow);
            }
        }
    }
}

void ItemPositionData::Restore()
{
    for (auto& [rect, pos] : mRects)
    {
        rect->RestoreRect(pos.rect);
        rect->setX(pos.x);
        rect->setY(pos.y);
    }

    for (auto& [line, pos] : mLines)
    {
        line->RestoreLine(pos.line);
        line->setX(pos.x);
        line->setY(pos.y);
    }
}

void ItemPositionData::AddRect(ResizeableRectItem* pItem)
{
    mRects[pItem] = { pItem->x(), pItem->y(), pItem->SaveRect() };
}

void ItemPositionData::AddLine(ResizeableArrowItem* pItem)
{
    mLines[pItem] = { pItem->x(), pItem->y(), pItem->SaveLine() };
}
