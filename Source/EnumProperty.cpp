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
    mCombo = new QComboBox(pParent);
    for (auto& item : mEnum->mValues)
    {
        mCombo->addItem(item.c_str());
    }
    GetPropertyIndex();
    Refresh();

    if (mOldIdx != -1)
    {
        mCombo->setCurrentIndex(mOldIdx);
    }

    connect(mCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [pParent, this](int index)
        {
            if (index != -1 && mOldIdx != index)
            {
                mUndoStack.push(new ChangeEnumPropertyCommand(pParent, this->mProperty, this->mEnum, this->mOldIdx, index));
            }
            mOldIdx = index;
        });

    connect(mCombo, &QComboBox::destroyed, this, [this](QObject*)
        {
            this->mCombo = nullptr;
        });
    return mCombo;
}

void EnumProperty::Refresh()
{
    // Update the idx based on the enum value
    GetPropertyIndex();

    // Update the text to match the enum value via its index
    setText(1, mEnum->mValues[mOldIdx].c_str());

    if (mCombo)
    {
        // If the combo is open then set the correct item in the drop down
        mCombo->setCurrentIndex(mOldIdx);
    }
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
