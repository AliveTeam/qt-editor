#include "BasicTypeProperty.hpp"
#include "model.hpp"

BasicTypeProperty::BasicTypeProperty(QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, int propertyValue, BasicType* pBasicType) : PropertyTreeItemBase(pParent, QStringList{ propertyName, QString::number(propertyValue) }), mUndoStack(undoStack), mBasicType(pBasicType)
{
    mValue = propertyValue;
}

QWidget* BasicTypeProperty::CreateEditorWidget(PropertyTreeWidget* pParent)
{
    auto spin = new BigSpinBox(pParent);
    spin->setMax(mBasicType->mMaxValue);
    spin->setMin(mBasicType->mMinValue);
    spin->setValue(mValue);
    return spin;
}
