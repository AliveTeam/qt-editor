#include "messageeditordialog.hpp"
#include "ui_messageeditordialog.h"

MessageEditorDialog::MessageEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageEditorDialog)
{
    ui->setupUi(this);
}

MessageEditorDialog::~MessageEditorDialog()
{
    delete ui;
}

void MessageEditorDialog::on_btnDeleteAll_clicked()
{

}


void MessageEditorDialog::on_btnDeleteSelected_clicked()
{

}


void MessageEditorDialog::on_btnUpdate_clicked()
{

}


void MessageEditorDialog::on_btnAdd_clicked()
{

}


void MessageEditorDialog::on_buttonBox_accepted()
{

}


void MessageEditorDialog::on_buttonBox_rejected()
{

}


void MessageEditorDialog::on_listWidget_itemSelectionChanged()
{

}

