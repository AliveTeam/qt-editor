#pragma once

#include <QMainWindow>
#include <QDockWidget>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QUndoStack>
#include <QPainter>
#include <QTreeWidget>
#include <QApplication>
#include <memory>
#include "model.hpp"

namespace Ui
{
    class EditorTab;
}

class EditorGraphicsScene;
class ResizeableArrowItem;
class ResizeableRectItem;
class CameraGraphicsItem;
struct MapObject;
class CollisionObject;
class CameraManager;

class EditorTab final : public QMainWindow
{
    Q_OBJECT
public:
    EditorTab(QTabWidget* aParent, UP_Model model, QString jsonFileName, bool isTempFile);
    ~EditorTab();
    void ZoomIn();
    void ZoomOut();
    void ResetZoom();
    bool Save();
    bool SaveAs();
    void Export();
    QString GetJsonFileName() const { return mJsonFileName; }
    Model& GetModel() const { return *mModel; }
    void ClearPropertyEditor();
    void PopulatePropertyEditor(QGraphicsItem* pItem);
    void Undo();
    void Redo();
    void wheelEvent(QWheelEvent* pEvent) override;
    EditorGraphicsScene& GetScene() 
    {
        return *mScene;
    }

    void EditHintFlyMessages();
    void EditLEDMessages();
    void EditPathData();
    void EditMapSize();

    void AddCommand(QUndoCommand* pCmd)
    {
        mUndoStack.push(pCmd);
    }

    bool IsClean() const
    {
        return !mIsTempFile && mUndoStack.isClean();
    }

    void UpdateCleanState();
    void UpdateTabTitle(bool clean);

    void AddObject();
    void AddCollision();

    ResizeableRectItem* MakeResizeableRectItem(MapObject* pMapObject);
    ResizeableArrowItem* MakeResizeableArrowItem(CollisionObject* pCollisionObject);
    CameraGraphicsItem* MakeCameraGraphicsItem(Camera* pCamera, int x, int y, int w, int h);

    CameraManager* GetCameraManagerDialog()
    {
        return mCameraManager;
    }

    void SetCameraManagerDialog(CameraManager* pDlg)
    {
        mCameraManager = pDlg;
    }

    void SyncPropertyEditor();

signals:
    void CleanChanged();

private slots:

    void cleanChanged(bool clean);

private:
    bool DoSave(QString fileName);

    Ui::EditorTab* ui = nullptr;
    float iZoomLevel = 1.0f;
    UP_Model mModel;
    QUndoStack mUndoStack;
    std::unique_ptr<EditorGraphicsScene> mScene;
    QString mJsonFileName;

    QString mExportedPathLvlName;
    QTabWidget* mParent = nullptr;
    bool mIsTempFile = false;

    CameraManager* mCameraManager = nullptr;
};
