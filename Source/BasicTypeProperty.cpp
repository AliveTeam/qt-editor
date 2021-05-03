#include "BasicTypeProperty.hpp"
#include "model.hpp"

BasicTypeProperty::BasicTypeProperty(QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, ObjectProperty* pProperty, BasicType* pBasicType) : PropertyTreeItemBase(pParent, QStringList{ propertyName, QString::number(pProperty->mBasicTypeValue) }), mUndoStack(undoStack), mProperty(pProperty), mBasicType(pBasicType)
{

}

QWidget* BasicTypeProperty::CreateEditorWidget(PropertyTreeWidget* pParent)
{
    auto spin = new BigSpinBox(pParent);
    spin->setMax(mBasicType->mMaxValue);
    spin->setMin(mBasicType->mMinValue);
    spin->setValue(mProperty->mBasicTypeValue);
    return spin;
}
