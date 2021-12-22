#include "editortab.hpp"
#include "ui_editortab.h"
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QOpenGLWidget>
#include <QUndoCommand>
#include <QSpinBox>
#include <QMenu>
#include <QFileDialog>
#include "resizeablearrowitem.hpp"
#include "resizeablerectitem.hpp"
#include "CameraGraphicsItem.hpp"
#include "EditorGraphicsScene.hpp"
#include "bigspinbox.hpp"
#include <QLineEdit>
#include "PropertyTreeItemBase.hpp"
#include "StringProperty.hpp"
#include "PropertyTreeWidget.hpp"
#include <QFileInfo>
#include "BasicTypeProperty.hpp"
#include "EnumProperty.hpp"
#include "cameramanager.hpp"
#include "exportpathdialog.hpp"
#include "easylogging++.h"
#include "changemapsizedialog.hpp"
#include "messageeditordialog.hpp"
#include "pathdataeditordialog.hpp"
#include "addobjectdialog.hpp"

// Zoom by 10% each time.
const float KZoomFactor = 0.10f;
const float KMaxZoomOutLevels = 5.0f;
const float KMaxZoomInLevels = 14.0f;

INITIALIZE_EASYLOGGINGPP

class SetSelectionCommand : public QUndoCommand
{
public:
    SetSelectionCommand(EditorTab* pTab, QGraphicsScene* pScene, QList<QGraphicsItem*>& oldSelection, QList<QGraphicsItem*>& newSelection) 
      : mTab(pTab),
        mScene(pScene),
        mOldSelection(oldSelection),
        mNewSelection(newSelection)
    {
        mFirst = true;
        if (mNewSelection.count() > 0)
        {
            setText(QString("Select %1 item(s)").arg(mNewSelection.count()));
        }
        else
        {
            setText("Clear selection");
        }
    }

    void redo() override
    {
        if (!mFirst)
        {
            mScene->clearSelection();
            for (auto& item : mNewSelection)
            {
                item->setSelected(true);
            }
            mScene->update();
        }
        mFirst = false;
        mTab->SyncPropertyEditor();
    }

    void undo() override
    {
        mScene->clearSelection();
        for (auto& item : mOldSelection)
        {
            item->setSelected(true);
        }
        mScene->update();
        mTab->SyncPropertyEditor();
    }

private:
    EditorTab* mTab = nullptr;
    QGraphicsScene* mScene = nullptr;
    QList<QGraphicsItem*> mOldSelection;
    QList<QGraphicsItem*> mNewSelection;
    bool mFirst = false;
};


class MoveItemsCommand : public QUndoCommand
{
public:
    MoveItemsCommand(QGraphicsScene* pScene, ItemPositionData oldPositions, ItemPositionData newPositions, Model& model)
        : mScene(pScene),
        mOldPositions(oldPositions),
        mNewPositions(newPositions),
        mModel(model)
    {
        mFirst = true;

        if (mNewPositions.Count() == 1)
        {
            auto pNewLine = mNewPositions.FirstLinePos();
            auto pOldRect = mOldPositions.FirstRectPos();
            if (pNewLine)
            {
                auto pOldLine = mOldPositions.FirstLinePos();
                const bool posChange = pOldLine->x != pNewLine->x || pOldLine->y != pNewLine->y;
                const bool lineChanged = pOldLine->line != pNewLine->line;
                if (posChange && lineChanged)
                {
                    setText(QString("Move and resize collision"));
                }
                else if (posChange && !lineChanged)
                {
                    setText(QString("Move collision"));
                }
                else
                {
                    setText(QString("Move collision point"));
                }
            }
            else
            {
                auto pNewRect = mNewPositions.FirstRectPos();
                const bool posChange = pOldRect->x != pNewRect->x || pOldRect->y != pNewRect->y;
                const bool rectChanged = pNewRect->rect != pOldRect->rect;
                if (posChange && rectChanged)
                {
                    setText(QString("Move and resize map object"));
                }
                else if (posChange && !rectChanged)
                {
                    setText(QString("Move map object"));
                }
                else
                {
                    setText(QString("Resize map object"));
                }
            }
        }
        else
        {
            setText(QString("Move %1 item(s)").arg(mNewPositions.Count()));
        }
    }

    void redo() override
    {
        if (!mFirst)
        {
            mNewPositions.Restore(mModel);
            mScene->update();
        }
        mFirst = false;
    }

    void undo() override
    {
        mOldPositions.Restore(mModel);
        mScene->update();
    }

private:
    QGraphicsScene* mScene = nullptr;
    ItemPositionData mOldPositions;
    ItemPositionData mNewPositions;
    Model& mModel;
    bool mFirst = false;
};

class EditorGraphicsView : public QGraphicsView
{
public:
    EditorGraphicsView(EditorTab* editorTab)
        : mEditorTab(editorTab)
    {

    }

    void mousePressEvent(QMouseEvent* pEvent) override
    {
        if (pEvent->button() != Qt::LeftButton)
        {
            // prevent band dragging on other buttons
            qDebug() << "Ignore non left press";
            pEvent->ignore();
            return;
        }

        qDebug() << "view mouse press (left)";
        QGraphicsView::mousePressEvent(pEvent);
    }

    void mouseReleaseEvent(QMouseEvent* pEvent) override
    {
        if (pEvent->button() != Qt::LeftButton)
        {
            qDebug() << "Ignore non left release";
            pEvent->ignore();
            return;
        }

        qDebug() << "view mouse release (left)";
        QGraphicsView::mouseReleaseEvent(pEvent);
    }

    void wheelEvent(QWheelEvent* pEvent) override
    {
        if (pEvent->modifiers() == Qt::Modifier::CTRL)
        {
            pEvent->ignore();
            return;
        }
        QGraphicsView::wheelEvent(pEvent);
    }

    void keyPressEvent(QKeyEvent* pEvent) override
    {
        if (pEvent->key() == Qt::Key::Key_Shift)
        {
            setDragMode(DragMode::ScrollHandDrag);
            pEvent->ignore();
            return;
        }
        QGraphicsView::keyPressEvent(pEvent);
    }

    void keyReleaseEvent(QKeyEvent* pEvent) override
    {
        if (pEvent->key() == Qt::Key::Key_Shift)
        {
            setDragMode(DragMode::RubberBandDrag);
            pEvent->ignore();
            return;
        }
        QGraphicsView::keyPressEvent(pEvent);
    }

    void contextMenuEvent(QContextMenuEvent* pEvent) override
    {
        QMenu menu(this);
        auto pAction = new QAction("Edit camera", &menu);
        connect(pAction, &QAction::triggered, this, [&]()
            {
                const QPoint scenePos = mapToScene(pEvent->pos()).toPoint();
                CameraManager cameraManager(this, mEditorTab, &scenePos);
                cameraManager.exec();
            });
        menu.addAction(pAction);
        menu.exec(pEvent->globalPos());
    }

private:
    EditorTab* mEditorTab = nullptr;
};


EditorTab::EditorTab(QTabWidget* aParent, UP_Model model, QString jsonFileName, bool isTempFile)
    : QMainWindow(aParent),
    ui(new Ui::EditorTab),
    mModel(std::move(model)),
    mJsonFileName(jsonFileName),
    mParent(aParent),
    mIsTempFile(isTempFile)
{
    ui->setupUi(this);

    // TODO: Set as a promoted type
    delete ui->graphicsView;
    ui->graphicsView = new EditorGraphicsView(this);
    QGraphicsView* pView = ui->graphicsView;
    pView->setDragMode(QGraphicsView::RubberBandDrag);

    pView->setRenderHint(QPainter::SmoothPixmapTransform);
    pView->setRenderHint(QPainter::Antialiasing);
    pView->setRenderHint(QPainter::TextAntialiasing);

    // turn off for now because of performance issues when a lot of
    // objects are onscreen
    //pView->setViewport(new QOpenGLWidget()); // Becomes owned by the view

    mScene = std::make_unique<EditorGraphicsScene>(*mModel);

    connect(mScene.get(), &EditorGraphicsScene::SelectionChanged, this, [&](QList<QGraphicsItem*> oldSelection, QList<QGraphicsItem*> newSelection)
        {
            mUndoStack.push(new SetSelectionCommand(this, mScene.get(), oldSelection, newSelection));
        });

    connect(mScene.get(), &EditorGraphicsScene::ItemsMoved, this, [&](ItemPositionData oldPositions, ItemPositionData newPositions)
        {
            mUndoStack.push(new MoveItemsCommand(mScene.get(), oldPositions, newPositions, *mModel));
        });

    connect(&mUndoStack, &QUndoStack::cleanChanged, this, &EditorTab::cleanChanged);

    iZoomLevel = 1.0f;
    for (int i = 0; i < 2; ++i)
    {
        //  ZoomIn();
    }

    setCentralWidget(ui->graphicsView);

    // TODO: Temp hack
    delete ui->treeWidget;
    ui->treeWidget = new PropertyTreeWidget(ui->dockWidgetContents_2);
    ui->verticalLayout_5->addWidget(ui->treeWidget);

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
                    auto pMapObject = MakeResizeableRectItem(mapObj.get());
                    mScene->addItem(pMapObject);
                }
            }
        }
    }

    for (auto& collision : mModel->CollisionItems())
    {
        auto pLine = new ResizeableArrowItem(pView, collision.get(), *static_cast<PropertyTreeWidget*>(ui->treeWidget));
        mScene->addItem(pLine);
    }

    const int kXMargin = 100;
    const int kYMargin = 100;
    mScene->setSceneRect(-kXMargin, -kYMargin, (mapInfo.mXSize * mapInfo.mXGridSize) + (kXMargin * 2), (mapInfo.mYSize * mapInfo.mYGridSize) + (kYMargin * 2));


    ui->graphicsView->setScene(mScene.get());

    mUndoStack.setUndoLimit(100);
    ui->undoView->setStack(&mUndoStack);

    static_cast<PropertyTreeWidget*>(ui->treeWidget)->Init();

    addDockWidget(Qt::RightDockWidgetArea, ui->propertyDockWidget);
    addDockWidget(Qt::RightDockWidgetArea, ui->undoHistoryDockWidget);

    ui->propertyDockWidget->setMinimumWidth(310);

    setContextMenuPolicy(Qt::PreventContextMenu);

    connect(&mUndoStack , &QUndoStack::cleanChanged, this, &EditorTab::UpdateTabTitle);
}

ResizeableRectItem* EditorTab::MakeResizeableRectItem(MapObject* pMapObject)
{
    return new ResizeableRectItem(ui->graphicsView, pMapObject, *static_cast<PropertyTreeWidget*>(ui->treeWidget));
}

void EditorTab::SyncPropertyEditor()
{
    auto selected = mScene->selectedItems();
    if (selected.count() == 1)
    {
        PopulatePropertyEditor(selected[0]);
    }
    else
    {
        ClearPropertyEditor();
    }
}

void EditorTab::cleanChanged(bool clean)
{
    UpdateCleanState();
}

void EditorTab::UpdateTabTitle(bool clean)
{
    QFileInfo fileInfo(mJsonFileName);
    QString title = fileInfo.fileName();
    if (!clean || mIsTempFile)
    {
        title += "*";
    }

    for (int i = 0; i < mParent->count(); i++)
    {
        if (mParent->widget(i) == this)
        {
            mParent->setTabText(i, title);
            break;
        }
    }
}

void EditorTab::wheelEvent(QWheelEvent* pEvent)
{
    if (pEvent->modifiers() == Qt::Modifier::CTRL)
    {
        if (pEvent->delta() > 0)
        {
            ZoomIn();
        }
        else
        {
            ZoomOut();
        }
    }
    QWidget::wheelEvent(pEvent);
}

// TODO: zoom doesn't work properly without ui->graphicsView->resetTransform().
// figure out why and if there's a better way.
void EditorTab::ZoomIn()
{
    if (iZoomLevel < 1.0f + (KZoomFactor*KMaxZoomInLevels))
    {
        iZoomLevel += KZoomFactor;
        ui->graphicsView->resetTransform();
        ui->graphicsView->scale(iZoomLevel, iZoomLevel);
    }
}

void EditorTab::ZoomOut()
{
    if (iZoomLevel > 1.0f - (KZoomFactor*KMaxZoomOutLevels))
    {
        iZoomLevel -= KZoomFactor;
        ui->graphicsView->resetTransform();
        ui->graphicsView->scale(iZoomLevel, iZoomLevel);
    }
}

void EditorTab::ResetZoom()
{
    iZoomLevel = 1.0f;
    ui->graphicsView->resetTransform();
    ui->graphicsView->scale(iZoomLevel, iZoomLevel);
}

EditorTab::~EditorTab()
{
    disconnect(&mUndoStack, &QUndoStack::cleanChanged, this, &EditorTab::UpdateTabTitle);
    delete ui;
}

void EditorTab::ClearPropertyEditor()
{
    auto pTree = static_cast<PropertyTreeWidget*>(ui->treeWidget);
    pTree->DePopulate();
}

void EditorTab::PopulatePropertyEditor(QGraphicsItem* pItem)
{
    ClearPropertyEditor();

    auto pTree = static_cast<PropertyTreeWidget*>(ui->treeWidget);
    pTree->Populate(*mModel, mUndoStack, pItem);
}

void EditorTab::Undo()
{
    mUndoStack.undo();
}

void EditorTab::Redo()
{
    mUndoStack.redo();
}

bool EditorTab::Save()
{
    if (mIsTempFile)
    {
        return SaveAs();
    }
    else
    {
        return DoSave(mJsonFileName);
    }
}

bool EditorTab::SaveAs()
{
    QString jsonSaveFileName = QFileDialog::getSaveFileName(this, tr("Save path json"), "", tr("Json files (*.json);;All Files (*)"));
    if (jsonSaveFileName.isEmpty())
    {
        // They didn't want to save it
        return false;
    }

    if (!DoSave(jsonSaveFileName))
    {
        return false;
    }

    // Saved OK, update the file name and tab title
    mJsonFileName = jsonSaveFileName;

    // No longer a temp file so don't force SaveAs next time
    if (mIsTempFile)
    {
        mIsTempFile = false;
        UpdateCleanState();
        UpdateTabTitle(true);
    }

    return true;
}

bool EditorTab::DoSave(QString fileName)
{
    std::string json = mModel->ToJson();
    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&f);
        stream << json.c_str();
        mUndoStack.setClean();
        return true;
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to save " + fileName);
        return false;
    }
}

void EditorTab::Export()
{
    auto exportDialog = new ExportPathDialog(this);
    // Get rid of "?"
    exportDialog->setWindowFlags(exportDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    exportDialog->setJsonPath(mJsonFileName);

    if (mExportedPathLvlName.isEmpty())
    {
        // will be XXPATH.BND, extract XX
        QString lvlName = QString::fromStdString(mModel->GetMapInfo().mPathBnd);
        if (lvlName.length() > 2)
        {
            lvlName = lvlName.left(2) + ".lvl";
        }
        exportDialog->setLvlName(lvlName);
    }
    else
    {
        exportDialog->setLvlName(mExportedPathLvlName);
    }

    exportDialog->exec();

    if (!exportDialog->getLvlName().isEmpty())
    {
        mExportedPathLvlName = exportDialog->getLvlName();
    }
}

void EditorTab::EditHintFlyMessages()
{
    auto pDlg = new MessageEditorDialog(this, this, *mModel, false);
    pDlg->exec();
}

void EditorTab::EditLEDMessages()
{
    auto pDlg = new MessageEditorDialog(this, this, *mModel, true);
    pDlg->exec();
}

void EditorTab::EditPathData()
{
    auto pDlg = new PathDataEditorDialog(this, this);
    pDlg->exec();
}

void EditorTab::EditMapSize()
{
    auto pDlg = new ChangeMapSizeDialog(this, this);
    pDlg->exec();
}

void EditorTab::UpdateCleanState()
{
    emit CleanChanged();
}

void EditorTab::AddObject()
{
    auto pDlg = new AddObjectDialog(this, this);
    pDlg->exec();
}
