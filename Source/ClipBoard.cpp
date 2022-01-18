#include "ClipBoard.hpp"
#include "editortab.hpp"
#include "resizeablearrowitem.hpp"
#include "resizeablerectitem.hpp"
#include "CameraGraphicsItem.hpp"

PasteItemsCommand::PasteItemsCommand(EditorTab* pTab, ClipBoard& clipBoard)
{
    setText("Paste items");

    // todo: Make another deep copy of the items
}

PasteItemsCommand::~PasteItemsCommand()
{
    // todo: delete copies if not added
}

void PasteItemsCommand::redo()
{
    // todo: Add/place items

    // todo: Re-calc parent camera
}

void PasteItemsCommand::undo()
{
    // todo: Remove items
}

ClipBoard::~ClipBoard()
{
    // todo: delete copies
}

void ClipBoard::Set(QList<QGraphicsItem*>& items, Model& model)
{
    if (items.isEmpty())
    {
        return;
    }

    mSourceGame = model.GetMapInfo().mGame;

    // Deep copy the items for pasting
    for (int i = 0; i < items.count(); i++)
    {
        QGraphicsItem* obj = items.at(i);
        auto pCameraGraphicsItem = qgraphicsitem_cast<CameraGraphicsItem*>(obj);
        if (pCameraGraphicsItem)
        {
            // todo:
        }
        else
        {
            auto pResizeableArrowItem = qgraphicsitem_cast<ResizeableArrowItem*>(obj);
            if (pResizeableArrowItem)
            {
                // todo:
                //mMapObjectGraphicsItems.push_back(new ResizeableArrowItem());
            }
        }
    }
}

bool ClipBoard::IsEmpty() const
{
    // todo
    return true;
}

const std::string& ClipBoard::SourceGame() const
{
    return mSourceGame;
}
