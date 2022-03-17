#ifndef EXPORTPATHDIALOG_HPP
#define EXPORTPATHDIALOG_HPP

#include <QDialog>

namespace Ui {
class ExportPathDialog;
}

class ExportPathDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportPathDialog(QWidget *parent = nullptr);
    ~ExportPathDialog();

    void setJsonPath(QString path);
    void setLvlName(QString path);
    void setRelivePath(QString path);
    QString getLvlName() const;
    QString getRelivePath() const;
private slots:
    void on_btnSelectJson_clicked();

    void on_btnSelectLvlFile_clicked();

    void on_btnSelectRelive_clicked();

    void on_btnExportAndRun_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ExportPathDialog *ui;
    void ExportToLvl();
};

#endif // EXPORTPATHDIALOG_HPP
