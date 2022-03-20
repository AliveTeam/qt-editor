#include "EditorMainWindow.hpp"
#include <QMessageBox>
#include <QPixmapCache>
#include "EditorTab.hpp"
#include "ui_EditorMainWindow.h"
#include "AboutDialog.hpp"
#include <QTabBar>
#include <QFileDialog>
#include <QInputDialog>
#include <QUuid>
#include "Model.hpp"
#include "PathSelectionDialog.hpp"
#include "ExportPathDialog.hpp"
#include "relive_api.hpp"
#include "EditorGraphicsScene.hpp"

EditorMainWindow::EditorMainWindow(QWidget* aParent)
    : QMainWindow(aParent),
    m_ui(new Ui::EditorMainWindow),
    m_Settings("Editor.ini", QSettings::IniFormat)
{
    //auto p = new AudioOutputPrimer();
    //p->start();

    // Construct the UI from the XML
    m_ui->setupUi(this);

    UpdateWindowTitle();

    m_ui->statusbar->showMessage(tr("Ready"));

    // Add short cuts to the tool bar.
    m_ui->toolBar->addAction(m_ui->action_open_path);
    m_ui->toolBar->addAction(m_ui->action_undo);
    m_ui->toolBar->addAction(m_ui->action_redo);
    m_ui->toolBar->addAction(m_ui->action_zoom_reset);
    m_ui->toolBar->addAction(m_ui->action_zoom_in);
    m_ui->toolBar->addAction(m_ui->action_zoom_out);

    connect(m_ui->tabWidget, &QTabWidget::tabCloseRequested, this, &EditorMainWindow::onCloseTab);

    this->m_ui->toolBar->setMovable(false);

    QPixmapCache::setCacheLimit(1024 * 50);

    QStringList files;
    files.append("C:/GitHub/qt-editor/build/Debug/level/OutputAE_ba.lvl_4.json");
    files.append("C:/GitHub/qt-editor/build/Debug/level/OutputAO_f1.lvl_2.json");
    files.append("C:\\Users\\paul\\Documents\\qt-editor\\OutputAE_mi.lvl_4.json");
    files.append("C:\\Users\\paul\\Documents\\qt-editor\\OutputAO_r2.lvl_4.json");

    for (const auto& file : files)
    {
        QFile f(file);
        if (f.exists())
        {
            onOpenPath(file, false);
        }
    }

    // Use full screen
    showMaximized();

    // Disable context menu on the QToolBar
    m_ui->toolBar->setContextMenuPolicy(Qt::PreventContextMenu);

    statusBar()->showMessage(tr("Ready"));
}

EditorMainWindow::~EditorMainWindow()
{
    m_Settings.sync();
    delete m_ui;
}


bool EditorMainWindow::onOpenPath(QString fullFileName, bool createNewPath)
{
    int newPathId = 0;
    bool isTempfile = false;
    std::optional<int> selectedPath;
    try
    {
        if (fullFileName.endsWith(".lvl", Qt::CaseInsensitive))
        {
            // Get the paths in the LVL
            ReliveAPI::EnumeratePathsResult ret = ReliveAPI::EnumeratePaths(fullFileName.toStdString());
            if (!createNewPath)
            {
                // Ask the user to pick one
                auto pathSelection = new PathSelectionDialog(this, ret);
                pathSelection->exec();

                selectedPath = pathSelection->SelectedPath();
                delete pathSelection;

                if (!selectedPath)
                {
                    // They didn't pick one
                    return false;
                }
            }
            else
            {
                if (ret.paths.empty())
                {
                    // The selected LVL had no path for some reason
                    QMessageBox::critical(this, "Error", "Selected LVL appears to contain no paths");
                    return false;
                }
                // Pick the first path to use as a template for the new path
                selectedPath = ret.paths[0];

                // And ask the user for the new path id
                bool ok = false;
                newPathId = QInputDialog::getInt(this, "Enter new path Id", "Path Id", 0, 0, 99, 1, &ok);
                if (!ok)
                {
                    // User bailed on picking a path id
                    return false;
                }
            }

            QUuid uuid = QUuid::createUuid();
            QString tempFileFullPath = QDir::toNativeSeparators(
                QDir::tempPath() + "/" +
                qApp->applicationName().replace(" ", "") +
                "_" +
                uuid.toString(QUuid::WithoutBraces) + ".json");

            // Convert the binary lvl path to json
            ReliveAPI::ExportPathBinaryToJson(tempFileFullPath.toStdString(), fullFileName.toStdString(), selectedPath.value());

            isTempfile = true;

            // And continue to load the newly saved json file
            fullFileName = tempFileFullPath;
        }
    }
    catch (const ReliveAPI::IOReadException& e)
    {
        QMessageBox::critical(this, "Error", QString("IO read failure: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::IOWriteException& e)
    {
        QMessageBox::critical(this, "Error", QString("IO write failure: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::UnknownEnumValueException& e)
    {
        QMessageBox::critical(this, "Error", QString("Unknown enum value: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::IOReadPastEOFException& e)
    {
        QMessageBox::critical(this, "Error", QString("IO read past EOF: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::EmptyPropertyNameException& e)
    {
        QMessageBox::critical(this, "Error", QString("Empty property name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::EmptyTypeNameException& e)
    {
        QMessageBox::critical(this, "Error", QString("Empty type name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::DuplicatePropertyKeyException& e)
    {
        QMessageBox::critical(this, "Error", QString("Duplicated property key: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::DuplicatePropertyNameException& e)
    {
        QMessageBox::critical(this, "Error", QString("Duplicated property name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::DuplicateEnumNameException& e)
    {
        QMessageBox::critical(this, "Error", QString("Duplicated enum name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::PropertyNotFoundException& e)
    {
        QMessageBox::critical(this, "Error", QString("Property not found: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::InvalidGameException& e)
    {
        QMessageBox::critical(this, "Error", QString("Invalid game name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::InvalidJsonException& e)
    {
        QMessageBox::critical(this, "Error", QString("Invalid json, can't parse: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::JsonVersionTooNew& e)
    {
        QMessageBox::critical(this, "Error", QString("Json version too new: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::JsonVersionTooOld& e)
    {
        QMessageBox::critical(this, "Error", QString("Json version too old: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::BadCameraNameException& e)
    {
        QMessageBox::critical(this, "Error", QString("Bad camera name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::JsonNeedsUpgradingException& e)
    {
        QMessageBox::critical(this, "Error", QString("Json needs upgrading: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::OpenPathException& e)
    {
        QMessageBox::critical(this, "Error", QString("Open path failure: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::CollisionsCountChangedException& e)
    {
        QMessageBox::critical(this, "Error", QString("Collision count changed: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::CameraOutOfBoundsException& e)
    {
        QMessageBox::critical(this, "Error", QString("Camera out of bounds: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::UnknownStructureTypeException& e)
    {
        QMessageBox::critical(this, "Error", QString("Unknown structure record: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::WrongTLVLengthException& e)
    {
        QMessageBox::critical(this, "Error", QString("TLV length is wrong: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::JsonKeyNotFoundException& e)
    {
        QMessageBox::critical(this, "Error", QString("Missing json key: ") + e.Key().c_str());
        return false;
    }
    catch (const ReliveAPI::Exception& e)
    {
        QMessageBox::critical(this, "Error", e.what().c_str());
        return false;
    }

    // First check if we already have this json file open
    for (int i = 0; i < m_ui->tabWidget->count(); i++)
    {
        auto pTab = static_cast<EditorTab*>(m_ui->tabWidget->widget(i));
        // TODO: Probably need to normalize slashes in here, C:\foo.txt vs C:/foo.txt
        // could get past this check.
        if (pTab->GetJsonFileName().compare(fullFileName, Qt::CaseInsensitive) == 0)
        {
            // Set focus to the tab
            m_ui->tabWidget->setCurrentIndex(i);
            return true;
        }
    }

    try
    {
        // Load the json file into the editors object model
        auto model = std::make_unique<Model>();
        model->LoadJson(fullFileName.toStdString());

        if (createNewPath)
        {
            model->CreateAsNewPath(newPathId);
        }

        // If exported to a temp file then delete it now we've loaded it to memory
        if (isTempfile)
        {
            QFile::remove(fullFileName);

            // Also change the file name to something more sane and force SaveAs if the user
            // attempts to save this path.
            const auto generatedName = model->GetMapInfo().mGame + "_" + model->GetMapInfo().mPathBnd + "_" + QString::number(*selectedPath).toStdString();
            fullFileName = QString(generatedName.c_str());
        }

        EditorTab* view = new EditorTab(m_ui->tabWidget, std::move(model), fullFileName, isTempfile, statusBar(), mSnapSettings);

        connect(
            view, &EditorTab::CleanChanged,
            this, &EditorMainWindow::UpdateWindowTitle
        );

        QFileInfo fileInfo(fullFileName);
        const int tabIdx = m_ui->tabWidget->addTab(view, fileInfo.fileName());
        m_ui->tabWidget->setTabToolTip(tabIdx, fullFileName);
        m_ui->tabWidget->setTabIcon(tabIdx, m_ui->action_open_path->icon());
        m_ui->tabWidget->setCurrentIndex(tabIdx);

        m_ui->stackedWidget->setCurrentIndex(1);

        view->UpdateTabTitle(view->IsClean());

        return true;
    }
    catch (const ModelException&)
    {
        QMessageBox::critical(this, "Error", "Failed to load json");
        return false;
    }
}

void EditorMainWindow::onCloseTab(int index)
{
    auto tab = static_cast<EditorTab*>(m_ui->tabWidget->widget(index));

    bool close = true;
    if (!tab->IsClean())
    {
        close = QMessageBox::question(this, "Confirm", "Close without saving changes?") == QMessageBox::Yes;
    }

    if (close)
    {
        tab->deleteLater();
        m_ui->tabWidget->removeTab(index);
        const int count = m_ui->tabWidget->tabBar()->count();
        if (count == 0)
        {
            m_ui->stackedWidget->setCurrentIndex(0);
        }
    }
}

void EditorMainWindow::on_action_about_qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void EditorMainWindow::on_action_about_triggered()
{
    auto about = new AboutDialog(this);
    about->exec();
    delete about;
}

void EditorMainWindow::on_action_open_path_triggered()
{
    QString lastOpenDir = m_Settings.value("last_open_dir").toString();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open level"), lastOpenDir, tr("Supported Files (*.json *.lvl);; Json Files (*.json);;Level Files (*.lvl);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        if (onOpenPath(fileName, false))
        {
            QFileInfo info(fileName);
            m_Settings.setValue("last_open_dir", info.dir().path());
        }
    }
}

static EditorTab* getActiveTab(QTabWidget* pTabWidget)
{
    const int idx = pTabWidget->currentIndex();
    if (idx != -1)
    {
        return static_cast<EditorTab*>(pTabWidget->widget(idx));
    }
    return nullptr;
}

void EditorMainWindow::on_action_zoom_reset_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->ResetZoom();
    }
}

void EditorMainWindow::on_action_zoom_in_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->ZoomIn();
    }
}

void EditorMainWindow::on_action_zoom_out_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->ZoomOut();
    }
}

void EditorMainWindow::on_action_undo_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->Undo();
    }
}

void EditorMainWindow::on_action_redo_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->Redo();
    }
}

void EditorMainWindow::on_action_save_path_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->Save();
    }
}

void EditorMainWindow::on_actionExport_to_lvl_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->Export();
    }
    else
    {
        auto exportDialog = new ExportPathDialog(this);
        exportDialog->exec();
        delete exportDialog;
    }
}


void EditorMainWindow::on_tabWidget_currentChanged(int /*index*/)
{
    UpdateWindowTitle();
}

void EditorMainWindow::UpdateWindowTitle()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    QString title = "Oddysee/Exoddus editor by Relive Team [https://aliveteam.github.io]";
    if (pTab)
    {
        title += " (" + pTab->GetJsonFileName();
        if (!pTab->IsClean())
        {
            title += "*";
        }
        title += ")";
    }
    setWindowTitle(title);
}

void EditorMainWindow::closeEvent(QCloseEvent* pEvent)
{
    if (m_ui->tabWidget->count() > 0)
    {
        bool anyTabsNeedSaving = false;
        for (int i = 0; i < m_ui->tabWidget->count(); i++)
        {
            if (!static_cast<EditorTab*>(m_ui->tabWidget->widget(i))->IsClean())
            {
                anyTabsNeedSaving = true;
                break;
            }
        }

        if (!anyTabsNeedSaving)
        {
            DisconnectTabSignals();

            m_ui->tabWidget->clear();
            pEvent->accept();
            return;
        }

        QMessageBox msgBox;
        QString strToShow = QString("Some paths have unsaved changes.");
        msgBox.setText(strToShow);
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);

        const int ret = msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Save:
        {
            for (int i = 0; i < m_ui->tabWidget->count(); i++)
            {
                if (!static_cast<EditorTab*>(m_ui->tabWidget->widget(i))->Save())
                {
                    pEvent->ignore();
                    return;
                }
            }

            // disconnect signals that will fire in our dtor and crash at shutdown
            DisconnectTabSignals();

            m_ui->tabWidget->clear();
            pEvent->accept();
            break;
        }
        case QMessageBox::Discard:
        {
            DisconnectTabSignals();

            m_ui->tabWidget->clear();
            pEvent->accept();
            break;
        }
        case QMessageBox::Cancel:
        {
            pEvent->ignore();
            break;
        }
        }
    }
    else
    {
        pEvent->accept();
    }
}

void EditorMainWindow::DisconnectTabSignals()
{
    for (int i = 0; i < m_ui->tabWidget->count(); i++)
    {
        auto pTab = static_cast<EditorTab*>(m_ui->tabWidget->widget(i));
        disconnect(
            pTab, &EditorTab::CleanChanged,
            this, &EditorMainWindow::UpdateWindowTitle
        );
    }
}

void EditorMainWindow::on_actionEdit_HintFly_messages_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->EditHintFlyMessages();
    }
}


void EditorMainWindow::on_actionEdit_LCDScreen_messages_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->EditLCDScreenMessages();
    }
}


void EditorMainWindow::on_actionEdit_path_data_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->EditPathData();
    }
}


void EditorMainWindow::on_actionEdit_map_size_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->EditMapSize();
    }
}

void EditorMainWindow::on_actionNew_path_triggered()
{
    QString lastOpenDir = m_Settings.value("last_open_dir").toString();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open lvl (as template for new path)"), lastOpenDir, tr("Level Files (*.lvl);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        if (onOpenPath(fileName, true))
        {
            QFileInfo info(fileName);
            m_Settings.setValue("last_open_dir", info.dir().path());
        }
    }
}

void EditorMainWindow::on_actionSave_As_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->SaveAs();
    }
}


void EditorMainWindow::on_actionAdd_object_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->AddObject();
    }
}


void EditorMainWindow::on_actionAdd_collision_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->AddCollision();
    }
}


void EditorMainWindow::on_action_close_path_triggered()
{
    const int idx = m_ui->tabWidget->currentIndex();
    if (idx != -1)
    {
        onCloseTab(idx);
    }
}

void EditorMainWindow::on_actionItem_transparency_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->EditTransparency();
    }
}


void EditorMainWindow::on_action_toggle_show_grid_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->GetScene().ToggleGrid();
    }
}


void EditorMainWindow::on_actionCut_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->Cut(mClipBoard);
    }
}


void EditorMainWindow::on_actionCopy_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->Copy(mClipBoard);
    }
}


void EditorMainWindow::on_actionPaste_triggered()
{
    if (!mClipBoard.IsEmpty())
    {
        EditorTab* pTab = getActiveTab(m_ui->tabWidget);
        if (pTab)
        {
            if (mClipBoard.SourceGame() != pTab->GetModel().GetMapInfo().mGame)
            {
                QMessageBox::critical(this, "Error", "You can't cut/copy paste data between AO and AE");
            }
            else
            {
                pTab->Paste(mClipBoard);
            }
        }
    }
}

void EditorMainWindow::on_action_snap_collision_items_on_x_toggled(bool on)
{
    mSnapSettings.CollisionSnapping().mSnapX = on;
}

void EditorMainWindow::on_action_snap_map_objects_x_toggled(bool on)
{
    mSnapSettings.MapObjectSnapping().mSnapX = on;
}

void EditorMainWindow::on_action_snap_collision_objects_on_y_toggled(bool on)
{
    mSnapSettings.CollisionSnapping().mSnapY = on;
}

void EditorMainWindow::on_action_snap_map_objects_y_toggled(bool on)
{
    mSnapSettings.MapObjectSnapping().mSnapY = on;
}
