#include "StringProperty.hpp"
#include <QComboBox>
#include "model.hpp"

StringProperty::StringProperty(MapObject* pMapObject, QUndoStack& undoStack, QTreeWidgetItem* pParent, QString propertyName, std::string* pProperty) 
    : PropertyTreeItemBase(pParent, QStringList{ propertyName, pProperty->c_str() }), mMapObject(pMapObject), mUndoStack(undoStack), mProperty(pProperty)
{
    mPrevValue = mProperty->c_str();
}

QWidget* StringProperty::CreateEditorWidget(PropertyTreeWidget* pParent)
{
    auto edit = new QLineEdit(pParent);
    edit->setText(mProperty->c_str());
    edit->setMaxLength(20);

    connect(edit, &QLineEdit::editingFinished, this, [this, edit, pParent]() 
        {
            if (mPrevValue != edit->text())
            {
                if (!edit->text().isEmpty())
                {
                    mUndoStack.push(new ChangeStringPropertyCommand(pParent, mProperty, text(0), mPrevValue, edit->text()));
                    mPrevValue = mProperty->c_str();
                    pParent->setItemWidget(this, 1, nullptr);
                }
            }
        });

    return edit;
}

void StringProperty::Refresh()
{
    setText(1, mProperty->c_str());
}

ChangeStringPropertyCommand::ChangeStringPropertyCommand(PropertyTreeWidget* pTreeWidget, std::string* pProperty, QString propertyName, QString oldValue, QString newValue) 
    : mTreeWidget(pTreeWidget), mProperty(pProperty), mOldValue(oldValue), mNewValue(newValue)
{
    setText(QString("Change property %1 from %2 to %3").arg(propertyName.trimmed(), oldValue, newValue));
}

void ChangeStringPropertyCommand::undo()
{
    *mProperty = mOldValue.toStdString();
    mTreeWidget->FindStringProperty(mProperty)->Refresh();
}

void ChangeStringPropertyCommand::redo()
{
    *mProperty = mNewValue.toStdString();
    mTreeWidget->FindStringProperty(mProperty)->Refresh();
}
