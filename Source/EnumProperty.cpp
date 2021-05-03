#include "EnumProperty.hpp"
#include "model.hpp"
#include <QComboBox>

EnumProperty::EnumProperty(QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, QString propertyValue, Enum* pEnum) : PropertyTreeItemBase(pParent, QStringList{ propertyName, propertyValue }), mUndoStack(undoStack), mEnum(pEnum)
{
    mValue = propertyValue;
}

QWidget* EnumProperty::CreateEditorWidget(PropertyTreeWidget* pParent)
{
    auto combo = new QComboBox(pParent);
    int i = 0;
    int idx = -1;
    for (auto& item : mEnum->mValues)
    {
        if (mValue == item.c_str())
        {
            idx = i;
        }
        combo->addItem(item.c_str());
        i++;
    }
    if (idx != -1)
    {
        combo->setCurrentIndex(idx);
    }
    return combo;
}