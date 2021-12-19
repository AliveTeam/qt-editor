#include "changemapsizedialog.hpp"
#include "ui_changemapsizedialog.h"
#include "editortab.hpp"
#include <QUndoCommand>

class ChangeMapSizeCommand final : public QUndoCommand
{
public:
    ChangeMapSizeCommand(EditorTab* pTab, int newXSize, int newYSize)
        : mTab(pTab),
        mNewXSize(newXSize),
        mNewYSize(newYSize)
    {
        mOldXSize = mTab->GetModel().GetMapInfo().mXSize;
        mOldYSize = mTab->GetModel().GetMapInfo().mYSize;

        setText("Change map size"); // TODO: Actually say what the old and new size is
    }

    void undo() override
    {
        // TODO:
    }

    void redo() override
    {
        // TODO:
    }

private:
    EditorTab* mTab = nullptr;

    int mOldXSize = 0;
    int mOldYSize = 0;

    int mNewXSize = 0;
    int mNewYSize = 0;
};

ChangeMapSizeDialog::ChangeMapSizeDialog(QWidget *parent, EditorTab* pTab) :
    QDialog(parent),
    ui(new Ui::ChangeMapSizeDialog),
    mTab(pTab)
{
    ui->setupUi(this);

    ui->spnXSize->setMinimum(1);
    ui->spnYSize->setMinimum(1);

    ui->spnXSize->setValue(mTab->GetModel().GetMapInfo().mXSize);
    ui->spnYSize->setValue(mTab->GetModel().GetMapInfo().mYSize);
}

ChangeMapSizeDialog::~ChangeMapSizeDialog()
{
    delete ui;
}

void ChangeMapSizeDialog::on_spnXSize_valueChanged(int arg1)
{

}


void ChangeMapSizeDialog::on_spnYSize_textChanged(const QString &arg1)
{
    if (mTab->GetModel().GetMapInfo().mXSize != ui->spnXSize->value() ||
        mTab->GetModel().GetMapInfo().mYSize != ui->spnYSize->value())
    {

    }
}


void ChangeMapSizeDialog::on_buttonBox_accepted()
{

}


void ChangeMapSizeDialog::on_buttonBox_rejected()
{

}

