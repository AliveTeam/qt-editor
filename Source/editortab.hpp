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

class EditorTab final : public QMainWindow
{
    Q_OBJECT
public:
    explicit EditorTab(QTabWidget* aParent, UP_Model model, QString jsonFileName);
    ~EditorTab();
    void ZoomIn();
    void ZoomOut();
    void ResetZoom();
    bool Save();
    void Export();
    QString GetJsonFileName() const { return mJsonFileName; }
    Model& GetModel() const { return *mModel; }
    QUndoStack& GetUndoStack() { return mUndoStack; }
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

private slots:
    void UpdateTabTitle(bool clean);

private:

    Ui::EditorTab* ui = nullptr;
    float iZoomLevel = 1.0f;
    UP_Model mModel;
    QUndoStack mUndoStack;
    std::unique_ptr<EditorGraphicsScene> mScene;
    QString mJsonFileName;

    QString mExportedPathLvlName;
    QTabWidget* mParent = nullptr;
};
