#include "editortab.hpp"
#include "ui_editortab.h"
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QOpenGLWidget>
#include "resizeablearrowitem.hpp"
#include "resizeablerectitem.hpp"

// Zoom by 10% each time.
const float KZoomFactor = 0.10f;
const float KMaxZoomOutLevels = 5.0f;
const float KMaxZoomInLevels = 14.0f;


class CameraGraphicsItem : public QGraphicsRectItem
{
public:
    CameraGraphicsItem(Camera* pCamera, int xpos, int ypos, int width, int height)
        : QGraphicsRectItem(xpos, ypos, width, height), mCamera(pCamera)
    {
        QPen pen;
        pen.setWidth(2);
        pen.setColor(QColor::fromRgb(255, 0, 255));
        setPen(pen);

        setZValue(1.0);
    }

    void paint(QPainter* aPainter, const QStyleOptionGraphicsItem* aOption, QWidget* aWidget) override
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

private:
    Camera* mCamera = nullptr;
};


class EditorGraphicsScene : public QGraphicsScene
{
public:
    EditorGraphicsScene()
    {
        CreateBackgroundBrush();
    }

    void CreateBackgroundBrush()
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

};


EditorTab::EditorTab(QWidget* aParent, UP_Model model)
    : QMainWindow(aParent),
    ui(new Ui::EditorTab),
    mModel(std::move(model))
{
    ui->setupUi(this);

    QGraphicsView* pView = ui->graphicsView;

    pView->setRenderHint(QPainter::SmoothPixmapTransform);
    pView->setRenderHint(QPainter::HighQualityAntialiasing);
    pView->setRenderHint(QPainter::Antialiasing);
    pView->setRenderHint(QPainter::TextAntialiasing);
    pView->setViewport(new QOpenGLWidget()); // Becomes owned by the view


    mScene = std::make_unique<EditorGraphicsScene>();

    iZoomLevel = 1.0f;
    for (int i = 0; i < 2; ++i)
    {
        //  ZoomIn();
    }

    setCentralWidget(ui->graphicsView);

    addDockWidget(Qt::RightDockWidgetArea, ui->propertyDockWidget);
    addDockWidget(Qt::RightDockWidgetArea, ui->undoHistoryDockWidget);

    // Disable "already disabled" context menus on the QDockWidgets
    ui->propertyDockWidget->setContextMenuPolicy(Qt::PreventContextMenu);
    ui->undoHistoryDockWidget->setContextMenuPolicy(Qt::PreventContextMenu);

    const MapInfo& mapInfo = mModel->GetMapInfo();

    for (int x = 0; x < mapInfo.mXSize; x++)
    {
        for (int y = 0; y < mapInfo.mYSize; y++)
        {
            Camera* pCam = mModel->CameraAt(x, y);
            auto pCameraGraphicsItem = new CameraGraphicsItem(pCam, mapInfo.mXGridSize * x, y *  mapInfo.mYGridSize, mapInfo.mXGridSize, mapInfo.mYGridSize);
            mScene->addItem(pCameraGraphicsItem);

            if (pCam)
            {
                for (auto& mapObj : pCam->mMapObjects)
                {
                    auto pMapObject = new ResizeableRectItem(pView, mapObj.get());
                    mScene->addItem(pMapObject);
                }
            }
        }
    }

    for (auto& collision : mModel->CollisionItems())
    {
        auto pLine = new ResizeableArrowItem(pView, collision.get());
        mScene->addItem(pLine);
    }

    const int kXMargin = 100;
    const int kYMargin = 100;
    mScene->setSceneRect(-kXMargin, -kYMargin, (mapInfo.mXSize * mapInfo.mXGridSize) + (kXMargin * 2), (mapInfo.mYSize * mapInfo.mYGridSize) + (kYMargin * 2));


    ui->graphicsView->setScene(mScene.get());

}

void EditorTab::ZoomIn()
{
    if (iZoomLevel < 1.0f + (KZoomFactor*KMaxZoomInLevels))
    {
        iZoomLevel += KZoomFactor;
        ui->graphicsView->scale(iZoomLevel, iZoomLevel);
    }
}

void EditorTab::ZoomOut()
{
    if (iZoomLevel > 1.0f - (KZoomFactor*KMaxZoomOutLevels))
    {
        iZoomLevel -= KZoomFactor;
        ui->graphicsView->scale(iZoomLevel, iZoomLevel);
    }
}

void EditorTab::ResetZoom()
{
    iZoomLevel = 1.0f;
    ui->graphicsView->scale(iZoomLevel, iZoomLevel);
}

EditorTab::~EditorTab()
{
    delete ui;
}
