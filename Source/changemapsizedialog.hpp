#ifndef CHANGEMAPSIZEDIALOG_HPP
#define CHANGEMAPSIZEDIALOG_HPP

#include <QDialog>

namespace Ui {
class ChangeMapSizeDialog;
}

class ChangeMapSizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeMapSizeDialog(QWidget *parent = nullptr);
    ~ChangeMapSizeDialog();

private slots:
    void on_spnXSize_valueChanged(int arg1);

    void on_spnYSize_textChanged(const QString &arg1);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ChangeMapSizeDialog *ui;
};

#endif // CHANGEMAPSIZEDIALOG_HPP
