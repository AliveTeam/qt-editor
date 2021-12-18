#include "editormainwindow.hpp"
#include <QMessageBox>
#include <QPixmapCache>
#include "editortab.hpp"
#include "ui_editormainwindow.h"
#include "aboutdialog.hpp"
#include <QTabBar>
#include <QFileDialog>
#include <QTemporaryFile>
#include <QInputDialog>
#include "model.hpp"
#include "pathselectiondialog.hpp"
#include "exportpathdialog.hpp"
#include "relive_api.hpp"

EditorMainWindow::EditorMainWindow(QWidget* aParent)
    : QMainWindow(aParent),
    m_ui(new Ui::EditorMainWindow),
    m_Settings("Editor.ini", QSettings::IniFormat)
{
    //auto p = new AudioOutputPrimer();
    //p->start();

    // Construct the UI from the XML
    m_ui->setupUi(this);

    UpdateWindowTitle(true);

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

    // Not implemented so remove for now

    delete m_ui->menuSnapping;
    delete m_ui->menuOptions;
}

EditorMainWindow::~EditorMainWindow()
{
    m_Settings.sync();
    delete m_ui;
}

bool EditorMainWindow::onOpenPath(QString fullFileName, bool createNewPath)
{
    int newPathId = 0;
    try
    {
        if (fullFileName.endsWith(".lvl", Qt::CaseInsensitive))
        {
            // Get the paths in the LVL
            ReliveAPI::EnumeratePathsResult ret = ReliveAPI::EnumeratePaths(fullFileName.toStdString());
            std::optional<int> selectedPath;
            if (!createNewPath)
            {
                // Ask the user to pick one
                auto pathSelection = new PathSelectionDialog(this, ret);
                // Get rid of "?"
                pathSelection->setWindowFlags(pathSelection->windowFlags() & ~Qt::WindowContextHelpButtonHint);
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

            // They picked one, now ask for the .json to save this path as
            QString jsonSaveFileName = QFileDialog::getSaveFileName(this, tr("Save path json"), "", tr("Json files (*.json);;All Files (*)"));
            if (jsonSaveFileName.isEmpty())
            {
                // They didn't want to save it
                return false;
            }

            // Convert the binary lvl path to json
            ReliveAPI::ExportPathBinaryToJson(jsonSaveFileName.toStdString(), fullFileName.toStdString(), selectedPath.value());

            // And continue to load the newly saved json file
            fullFileName = jsonSaveFileName;
        }
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

        EditorTab* view = new EditorTab(m_ui->tabWidget, std::move(model), fullFileName);

        connect(
            &view->GetUndoStack(), &QUndoStack::cleanChanged,
            this, &EditorMainWindow::UpdateWindowTitle
        );

        QFileInfo fileInfo(fullFileName);
        const int tabIdx = m_ui->tabWidget->addTab(view, fileInfo.fileName());
        m_ui->tabWidget->setTabToolTip(tabIdx, fullFileName);
        m_ui->tabWidget->setTabIcon(tabIdx, m_ui->action_open_path->icon());
        m_ui->tabWidget->setCurrentIndex(tabIdx);

        m_ui->stackedWidget->setCurrentIndex(1);
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
    if (!tab->GetUndoStack().isClean())
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open level"), lastOpenDir, tr("Json files (*.json);;Level files (*.lvl);;All Files (*)"));
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
        // Get rid of "?"
        exportDialog->setWindowFlags(exportDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        exportDialog->exec();
        delete exportDialog;
    }
}


void EditorMainWindow::on_tabWidget_currentChanged(int index)
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    UpdateWindowTitle(pTab ? pTab->GetUndoStack().isClean() : true);
}

void EditorMainWindow::UpdateWindowTitle(bool clean)
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    QString title = "Oddysee/Exoddus editor by Relive Team [https://aliveteam.github.io]";
    if (pTab)
    {
        title += " (" + pTab->GetJsonFileName();
        if (!clean)
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
            if (!static_cast<EditorTab*>(m_ui->tabWidget->widget(i))->GetUndoStack().isClean())
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
            &pTab->GetUndoStack(), &QUndoStack::cleanChanged,
            this, &EditorMainWindow::UpdateWindowTitle
        );
    }
}

void EditorMainWindow::on_actionEdit_HintFly_message_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->EditHintFlyMessages();
    }
}


void EditorMainWindow::on_actionEdit_LED_messages_triggered()
{
    EditorTab* pTab = getActiveTab(m_ui->tabWidget);
    if (pTab)
    {
        pTab->EditLEDMessages();
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open lvl (as template for new path)"), lastOpenDir, tr("Level files (*.lvl);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        if (onOpenPath(fileName, true))
        {
            QFileInfo info(fileName);
            m_Settings.setValue("last_open_dir", info.dir().path());
        }
    }
}
