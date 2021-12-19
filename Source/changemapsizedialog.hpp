#ifndef CHANGEMAPSIZEDIALOG_HPP
#define CHANGEMAPSIZEDIALOG_HPP

#include <QDialog>

namespace Ui {
class ChangeMapSizeDialog;
}

class EditorTab;

class ChangeMapSizeDialog : public QDialog
{
    Q_OBJECT

public:
    ChangeMapSizeDialog(QWidget *parent, EditorTab* pTab);
    ~ChangeMapSizeDialog();

private slots:
    void on_spnXSize_valueChanged(int arg1);

    void on_spnYSize_textChanged(const QString &arg1);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ChangeMapSizeDialog *ui;
    EditorTab* mTab = nullptr;
};

#endif // CHANGEMAPSIZEDIALOG_HPP
