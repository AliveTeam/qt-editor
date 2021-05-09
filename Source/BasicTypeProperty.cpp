#include "BasicTypeProperty.hpp"
#include "model.hpp"

ChangeBasicTypePropertyCommand::ChangeBasicTypePropertyCommand(PropertyTreeWidget* pTreeWidget, ObjectProperty* pProperty, BasicType* pBasicType, int oldValue, int newValue) : mTreeWidget(pTreeWidget), mProperty(pProperty), mBasicType(pBasicType), mOldValue(oldValue), mNewValue(newValue)
{
    setText(QString("Change property %1 from %2 to %3").arg(mProperty->mName.c_str(), QString::number(mOldValue), QString::number(mNewValue)));
}

void ChangeBasicTypePropertyCommand::undo()
{
    mProperty->mBasicTypeValue = mOldValue;
    mTreeWidget->FindObjectPropertyByKey(mProperty)->Refresh();
}

void ChangeBasicTypePropertyCommand::redo()
{
    mProperty->mBasicTypeValue = mNewValue;
    mTreeWidget->FindObjectPropertyByKey(mProperty)->Refresh();
}


BasicTypeProperty::BasicTypeProperty(QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, ObjectProperty* pProperty, BasicType* pBasicType) : PropertyTreeItemBase(pParent, QStringList{ propertyName, QString::number(pProperty->mBasicTypeValue) }), mUndoStack(undoStack), mProperty(pProperty), mBasicType(pBasicType)
{

}

QWidget* BasicTypeProperty::CreateEditorWidget(PropertyTreeWidget* pParent)
{
    mSpinBox = new BigSpinBox(pParent);
    mSpinBox->setMax(mBasicType->mMaxValue);
    mSpinBox->setMin(mBasicType->mMinValue);
    mSpinBox->setValue(mProperty->mBasicTypeValue);

    mOldValue = mProperty->mBasicTypeValue;

    connect(mSpinBox, &BigSpinBox::valueChanged, this, [pParent, this](qint64 newValue)
        {
            if (mOldValue != newValue)
            {
                mUndoStack.push(new ChangeBasicTypePropertyCommand(pParent, this->mProperty, this->mBasicType, this->mOldValue, newValue));
            }
            mOldValue = newValue;
        });

    connect(mSpinBox, &BigSpinBox::destroyed, this, [this](QObject*)
        {
            this->mSpinBox = nullptr;
        });

    return mSpinBox;
}

void BasicTypeProperty::Refresh()
{
    if (mSpinBox)
    {
        mSpinBox->setValue(mProperty->mBasicTypeValue, false);
    }
    else
    {
        setText(1, QString::number(mProperty->mBasicTypeValue));
    }
}
