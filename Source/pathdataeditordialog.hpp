#ifndef PATHDATAEDITORDIALOG_HPP
#define PATHDATAEDITORDIALOG_HPP

#include <QDialog>

namespace Ui {
class PathDataEditorDialog;
}

class PathDataEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathDataEditorDialog(QWidget *parent = nullptr);
    ~PathDataEditorDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::PathDataEditorDialog *ui;
};

#endif // PATHDATAEDITORDIALOG_HPP
