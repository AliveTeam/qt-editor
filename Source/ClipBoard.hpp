#pragma once

#include <QList>
#include <QGraphicsItem>
#include <QUndoCommand>
#include <string>

class EditorTab;
class Model;
class ClipBoard;
class ResizeableRectItem;

class PasteItemsCommand final : public QUndoCommand
{
public:
    PasteItemsCommand(EditorTab* pTab, ClipBoard& clipBoard);
    ~PasteItemsCommand();
    void redo() override;
    void undo() override;
private:
};

class ClipBoard final
{
public:
    ~ClipBoard();

    void Set(QList<QGraphicsItem*>& items, Model& model);
    bool IsEmpty() const;

    const std::string& SourceGame() const;
private:
    std::string mSourceGame;

    QList<ResizeableRectItem*> mMapObjectGraphicsItems;
};
