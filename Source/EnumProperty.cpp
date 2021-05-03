#include "EnumProperty.hpp"
#include "model.hpp"
#include <QComboBox>


ChangeEnumPropertyCommand::ChangeEnumPropertyCommand(PropertyTreeWidget* pTreeWidget, ObjectProperty* pProperty, Enum* pEnum, int oldIdx, int newIdx)
    : mTreeWidget(pTreeWidget), mProperty(pProperty), mEnum(pEnum), mOldIdx(oldIdx), mNewIdx(newIdx)
{
    setText(QString("Change property %1 from %2 to %3").arg(mEnum->mName.c_str(), mEnum->mValues[mOldIdx].c_str(), mEnum->mValues[mNewIdx].c_str()));
}

void ChangeEnumPropertyCommand::undo()
{
    mProperty->mEnumValue = mEnum->mValues[mOldIdx];
    mTreeWidget->FindObjectPropertyByKey(mProperty)->Refresh();
}

void ChangeEnumPropertyCommand::redo()
{
    mProperty->mEnumValue = mEnum->mValues[mNewIdx];
    mTreeWidget->FindObjectPropertyByKey(mProperty)->Refresh();
}

EnumProperty::EnumProperty(QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, ObjectProperty* pProperty, Enum* pEnum) : PropertyTreeItemBase(pParent, QStringList{ propertyName, pProperty->mEnumValue.c_str() }), mUndoStack(undoStack), mProperty(pProperty), mEnum(pEnum)
{

}

QWidget* EnumProperty::CreateEditorWidget(PropertyTreeWidget* pParent)
{
    auto combo = new QComboBox(pParent);
    for (auto& item : mEnum->mValues)
    {
        combo->addItem(item.c_str());
    }
    GetPropertyIndex();
    Refresh();

    if (mOldIdx != -1)
    {
        combo->setCurrentIndex(mOldIdx);
    }

    connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [pParent, this](int index)
        {
            if (index != -1 && mOldIdx != index)
            {
                mUndoStack.push(new ChangeEnumPropertyCommand(pParent, this->mProperty, this->mEnum, this->mOldIdx, index));
            }
            mOldIdx = index;
        });

    return combo;
}

void EnumProperty::Refresh()
{
    setText(1, mEnum->mValues[mOldIdx].c_str());
    // TODO: Handle if the combo is open
}

void EnumProperty::GetPropertyIndex()
{
    mOldIdx = -1;
    int i = 0;
    for (auto& item : mEnum->mValues)
    {
        if (mProperty->mEnumValue == item.c_str())
        {
            mOldIdx = i;
            break;
        }
        i++;
    }
}
