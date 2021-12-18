#include "pathdataeditordialog.hpp"
#include "ui_pathdataeditordialog.h"

PathDataEditorDialog::PathDataEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PathDataEditorDialog)
{
    ui->setupUi(this);
}

PathDataEditorDialog::~PathDataEditorDialog()
{
    delete ui;
}

void PathDataEditorDialog::on_buttonBox_accepted()
{

}


void PathDataEditorDialog::on_buttonBox_rejected()
{

}

