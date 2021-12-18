#include "changemapsizedialog.hpp"
#include "ui_changemapsizedialog.h"

ChangeMapSizeDialog::ChangeMapSizeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeMapSizeDialog)
{
    ui->setupUi(this);
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

}


void ChangeMapSizeDialog::on_buttonBox_accepted()
{

}


void ChangeMapSizeDialog::on_buttonBox_rejected()
{

}

