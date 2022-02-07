#include "exportpathdialog.hpp"
#include "ui_exportpathdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QUuid>
#include "relive_api.hpp"

ExportPathDialog::ExportPathDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::ExportPathDialog)
{
    ui->setupUi(this);
}

ExportPathDialog::~ExportPathDialog()
{
    delete ui;
}

void ExportPathDialog::on_btnSelectJson_clicked()
{
    QString jsonFileName = QFileDialog::getOpenFileName(this, tr("Save path json"), "", tr("Json Files (*.json);;All Files (*)"));
    if (!jsonFileName.isEmpty())
    {
        setJsonPath(jsonFileName);
    }
}

void ExportPathDialog::on_btnSelectLvlFile_clicked()
{
    QString lvlFileName = QFileDialog::getOpenFileName(this, tr("Open level"), "", tr("Level Files (*.lvl);;All Files (*)"));
    if (!lvlFileName.isEmpty())
    {
        setLvlName(lvlFileName);
    }
}

void ExportPathDialog::on_buttonBox_accepted()
{
    try
    {
        std::vector<std::string> resourceSources; // TODO: Wire into UI
        
        QUuid uuid = QUuid::createUuid();
        QString tempFileFullPath = QDir::toNativeSeparators(
            QDir::tempPath() + "/" + 
            qApp->applicationName().replace(" ", "") +
            "_" +
            uuid.toString(QUuid::WithoutBraces) + ".lvl.tmp");

        // Export to a temp lvl file
        ReliveAPI::ImportPathJsonToBinary(
            ui->txtJsonPath->text().toStdString(),
            ui->txtLvlFilePath->text().toStdString(),
            tempFileFullPath.toStdString(),
            resourceSources);

        // Then overwrite the original lvl with the temp one
        if (!QFile::remove(ui->txtLvlFilePath->text()))
        {
            QMessageBox::critical(this, "Error", "Failed to delete " + ui->txtLvlFilePath->text() + " in order to replace it with the updated lvl");
            QFile::remove(tempFileFullPath);
            return;
        }

        if (!QFile::rename(tempFileFullPath, ui->txtLvlFilePath->text()))
        {
            QMessageBox::critical(this, "Error", "Failed to rename from " + tempFileFullPath + " to " + ui->txtLvlFilePath->text());
            return;
        }
    }
    catch (ReliveAPI::Exception& e)
    {
        QMessageBox::critical(this, "Error", e.what().c_str());
    }
}

void ExportPathDialog::on_buttonBox_rejected()
{
    ui->txtLvlFilePath->clear(); // don't persist the bad path
}

void ExportPathDialog::setJsonPath(QString path)
{
    ui->txtJsonPath->setText(path);
}

void ExportPathDialog::setLvlName(QString path)
{
    ui->txtLvlFilePath->setText(path);
}

QString ExportPathDialog::getLvlName() const
{
    return ui->txtLvlFilePath->text();
}
