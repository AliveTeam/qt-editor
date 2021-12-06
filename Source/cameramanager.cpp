#include "cameramanager.hpp"
#include "ui_cameramanager.h"
#include "editortab.hpp"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

class CameraListItem final : public QListWidgetItem
{
public:
    CameraListItem(QListWidget* pParent, Camera* pCamera)
        : QListWidgetItem(QString(pCamera->mName.c_str()), pParent), mCamera(pCamera)
    {

    }

    const Camera* GetCamera() const
    {
        return mCamera;
    }
private:
    Camera* mCamera = nullptr;
};

CameraManager::CameraManager(QWidget *parent, EditorTab* pParentTab, const QPoint* openedPos) :
    QDialog(parent),
    ui(new Ui::CameraManager),
    mTab(pParentTab)
{
    ui->setupUi(this);

    const MapInfo& mapInfo = mTab->GetModel().GetMapInfo();

    const auto& cameras = mTab->GetModel().GetCameras();
    for (const auto& cam : cameras)
    {
        ui->listWidget->addItem(new CameraListItem(ui->listWidget, cam.get()));
    }

    if (openedPos)
    {
        // work out which camera x,y was clicked on
        const int camX = openedPos->x() / mapInfo.mXGridSize;
        const int camY = openedPos->y() / mapInfo.mYGridSize;
        qDebug() << "Looking for a camera at " << camX << " , " << camY;

        const auto pCamera = mTab->GetModel().CameraAt(camX, camY);
        if (pCamera)
        {
            qDebug() << "Got a camera at " << camX << " , " << camY << " " << pCamera->mName.c_str();
            for (int i = 0; i < ui->listWidget->count(); i++)
            {
                auto pItem = static_cast<CameraListItem*>(ui->listWidget->item(i));
                if (pCamera == pItem->GetCamera())
                {
                    ui->listWidget->clearSelection();
                    pItem->setSelected(true);
                    break;
                }
            }
        }
    }
}

CameraManager::~CameraManager()
{
    delete ui;
}

void CameraManager::on_btnSelectImage_clicked()
{
    if (!ui->listWidget->selectedItems().empty())
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open level"), "", tr("PNG image files (*.png);;"));
        if (!fileName.isEmpty())
        {
            QPixmap img(fileName);
            if (img.isNull())
            {
                QMessageBox::critical(this, "Error", "Failed to load image");
                return;
            }

            if (img.width() != 640 || img.height() != 240)
            {
                img = img.scaled(640, 240);
                if (img.isNull())
                {
                    QMessageBox::critical(this, "Error", "Failed to resize image");
                    return;
                }
            }

            auto pItem = static_cast<CameraListItem*>(ui->listWidget->selectedItems()[0]);

            // TODO: Set Base64 encoded data via a QUndoAction + sync the CameraGraphicsItem
            //pItem->GetCamera()->mCameraImageandLayers.mCameraImage;

        }
    }
}

void CameraManager::on_buttonBox_accepted()
{

}

void CameraManager::on_buttonBox_rejected()
{

}
