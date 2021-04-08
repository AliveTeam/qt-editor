#pragma once

#include <QDialog>
#include "modthread.hpp"

namespace Ui
{
    class AboutDialog;
}

class AboutDialog final : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();
private:
    void stopMusic();
    void startMusic();
private:
    Ui::AboutDialog *ui;
    ModThread* mThread = nullptr;
};
