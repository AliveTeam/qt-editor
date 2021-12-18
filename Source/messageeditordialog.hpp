#ifndef MESSAGEEDITORDIALOG_HPP
#define MESSAGEEDITORDIALOG_HPP

#include <QDialog>

namespace Ui {
class MessageEditorDialog;
}

class MessageEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageEditorDialog(QWidget *parent = nullptr);
    ~MessageEditorDialog();

private slots:
    void on_btnDeleteAll_clicked();

    void on_btnDeleteSelected_clicked();

    void on_btnUpdate_clicked();

    void on_btnAdd_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_listWidget_itemSelectionChanged();

private:
    Ui::MessageEditorDialog *ui;
};

#endif // MESSAGEEDITORDIALOG_HPP
