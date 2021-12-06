#ifndef CAMERAMANAGER_HPP
#define CAMERAMANAGER_HPP

#include <QDialog>
#include <QPoint>

namespace Ui {
class CameraManager;
}

class EditorTab;

class CameraManager : public QDialog
{
    Q_OBJECT

public:
    CameraManager(QWidget *parent, EditorTab* pParentTab, const QPoint* openedPos);
    ~CameraManager();

private slots:
    void on_btnSelectImage_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::CameraManager *ui;
    EditorTab* mTab = nullptr;
};

#endif // CAMERAMANAGER_HPP
