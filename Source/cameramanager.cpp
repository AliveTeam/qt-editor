#include "cameramanager.hpp"
#include "ui_cameramanager.h"
#include "editortab.hpp"
#include "EditorGraphicsScene.hpp"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QBuffer>
#include <QGraphicsItem>
#include "CameraGraphicsItem.hpp"

class ChangeCameraImageCommand final : public QUndoCommand
{
public:
    ChangeCameraImageCommand(CameraGraphicsItem* pCameraGraphicsItem, QPixmap newImage, EditorTab* pEditorTab)
        : mCameraGraphicsItem(pCameraGraphicsItem), mNewImage(newImage), mEditorTab(pEditorTab)
    {
        // todo: set correctly
        setText("Change camera image at " + QString::number(mCameraGraphicsItem->GetCamera()->mY) + "," + QString::number(mCameraGraphicsItem->GetCamera()->mX));
        mOldImage = pCameraGraphicsItem->GetImage();
    }

    void undo() override
    {
        mCameraGraphicsItem->SetImage(mOldImage);
        mEditorTab->GetScene().invalidate();
        UpdateModel(mOldImage);
    }

    void redo() override
    {
        mCameraGraphicsItem->SetImage(mNewImage);
        mEditorTab->GetScene().invalidate();
        UpdateModel(mNewImage);
    }

private:
    void UpdateModel(QPixmap img)
    {
        QPixmap pixmap;
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        img.save(&buffer, "PNG");
        buffer.close();

        mCameraGraphicsItem->GetCamera()->mCameraImageandLayers.mCameraImage = bytes.toBase64().toStdString();
    }

    CameraGraphicsItem* mCameraGraphicsItem;
    EditorTab* mEditorTab;

    QPixmap mNewImage;
    QPixmap mOldImage;
};

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
    // TODO: Need place holders for blocks that don't have a model item yet

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
            CameraGraphicsItem* pCameraGraphicsItem = CameraGraphicsItemByModelPtr(pItem->GetCamera());

            mTab->GetUndoStack().push(new ChangeCameraImageCommand(pCameraGraphicsItem, img, mTab));
        }
    }
}

void CameraManager::on_buttonBox_accepted()
{

}

void CameraManager::on_buttonBox_rejected()
{

}

CameraGraphicsItem* CameraManager::CameraGraphicsItemByPos(const QPoint& pos)
{
    QList<QGraphicsItem*> itemsAtPos = mTab->GetScene().items(pos);
    CameraGraphicsItem* pCameraGraphicsItem = nullptr;
    for (int i = 0; i < itemsAtPos.count(); i++)
    {
        pCameraGraphicsItem = qgraphicsitem_cast<CameraGraphicsItem*>(itemsAtPos.at(i));
        if (pCameraGraphicsItem)
        {
            break;
        }
    }
    return pCameraGraphicsItem;
}   

CameraGraphicsItem* CameraManager::CameraGraphicsItemByModelPtr(const Camera* cam)
{
    QList<QGraphicsItem*> itemsAtPos = mTab->GetScene().items();
    for (int i = 0; i < itemsAtPos.count(); i++)
    {
        CameraGraphicsItem* pCameraGraphicsItem = qgraphicsitem_cast<CameraGraphicsItem*>(itemsAtPos.at(i));
        if (pCameraGraphicsItem)
        {
            if (pCameraGraphicsItem->GetCamera() == cam)
            {
                return pCameraGraphicsItem;
            }
        }
    }
    return nullptr;
}
