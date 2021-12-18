#include "messageeditordialog.hpp"
#include "ui_messageeditordialog.h"
#include "model.hpp"
#include "editortab.hpp"
#include <QUndoCommand>

class ChangeMessagesCommand final : public QUndoCommand
{
public:
    ChangeMessagesCommand(Model& model, const std::vector<std::string>& oldMsgs, const std::vector<std::string>& newMsgs, bool isLedMsgs)
        : mModel(model),
        mOldMsgs(oldMsgs),
        mNewMsgs(newMsgs),
        mIsLedMsgs(isLedMsgs)
    {
        if (mIsLedMsgs)
        {
            setText("Change LED messages");
        }
        else
        {
            setText("Change hint fly messages");
        }
    }

    void undo() override
    {
        if (mIsLedMsgs)
        {
            mModel.GetMapInfo().mLedMessages = mOldMsgs;
        }
        else
        {
            mModel.GetMapInfo().mHintFlyMessages = mOldMsgs;
        }
    }

    void redo() override
    {
        if (mIsLedMsgs)
        {
            mModel.GetMapInfo().mLedMessages = mNewMsgs;
        }
        else
        {
            mModel.GetMapInfo().mHintFlyMessages = mNewMsgs;
        }
    }

private:
    Model& mModel;
    const std::vector<std::string> mOldMsgs;
    const std::vector<std::string> mNewMsgs;
    bool mIsLedMsgs = false;
};

MessageEditorDialog::MessageEditorDialog(QWidget *parent, EditorTab* pTab, Model& model, bool isLedMsgs) :
    QDialog(parent),
    ui(new Ui::MessageEditorDialog),
    mTab(pTab),
    mModel(model),
    mIsLedMsgs(isLedMsgs)
{
    ui->setupUi(this);

    const auto& msgs = GetMsgs();
    for (const auto& msg : msgs)
    {
        ui->listWidget->addItem(msg.c_str());
    }
}

MessageEditorDialog::~MessageEditorDialog()
{
    delete ui;
}

void MessageEditorDialog::on_btnDeleteAll_clicked()
{
    ui->listWidget->clear();
}


void MessageEditorDialog::on_btnDeleteSelected_clicked()
{
    if (!ui->listWidget->selectedItems().empty())
    {
        delete ui->listWidget->takeItem(ui->listWidget->row(ui->listWidget->selectedItems().at(0)));
    }
}


void MessageEditorDialog::on_btnUpdate_clicked()
{
    if (!ui->txtMessage->text().isEmpty())
    {
        if (!ui->listWidget->selectedItems().empty())
        {
            ui->listWidget->selectedItems().at(0)->setText(ui->txtMessage->text());
        }
    }
}


void MessageEditorDialog::on_btnAdd_clicked()
{
    if (!ui->txtMessage->text().isEmpty())
    {
        ui->listWidget->addItem(ui->txtMessage->text());
    }
}

void MessageEditorDialog::on_buttonBox_accepted()
{
    const auto& oldMsgs = GetMsgs();
    std::vector<std::string> newMsgs;
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        newMsgs.emplace_back(ui->listWidget->item(i)->text().toStdString());
    }

    if (oldMsgs != newMsgs)
    {
        auto cmd = new ChangeMessagesCommand(mModel, oldMsgs, newMsgs, mIsLedMsgs);
        mTab->AddCommand(cmd);
    }
}


void MessageEditorDialog::on_buttonBox_rejected()
{
    // nothing to do
}

void MessageEditorDialog::on_listWidget_itemSelectionChanged()
{
    if (!ui->listWidget->selectedItems().empty())
    {
        ui->txtMessage->setText(ui->listWidget->selectedItems().at(0)->text());
    }
}

std::vector<std::string>& MessageEditorDialog::GetMsgs()
{
    if (mIsLedMsgs)
    {
        return mModel.GetMapInfo().mLedMessages;
    }
    else
    {
        return mModel.GetMapInfo().mHintFlyMessages;
    }
}
