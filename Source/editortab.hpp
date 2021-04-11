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
    explicit EditorTab(QWidget* aParent, UP_Model model);
    ~EditorTab();
    void ZoomIn();
    void ZoomOut();
    void ResetZoom();
    Model& GetModel() const { return *mModel; }
    void ClearPropertyEditor();
    void PopulatePropertyEditor(QGraphicsItem* pItem);
private:
    Ui::EditorTab* ui = nullptr;
    float iZoomLevel = 1.0f;
    UP_Model mModel;
    QUndoStack mUndoStack;
    std::unique_ptr<EditorGraphicsScene> mScene;
};
