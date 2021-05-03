#include "editormainwindow.hpp"
#include <QMessageBox>
#include <QPixmapCache>
#include "editortab.hpp"
#include "ui_editormainwindow.h"
#include "aboutdialog.hpp"
#include <QTabBar>
#include <QFileDialog>
#include "model.hpp"

EditorMainWindow::EditorMainWindow(QWidget* aParent)
    : QMainWindow(aParent),
    m_ui(new Ui::EditorMainWindow),
    m_Settings("Editor.ini", QSettings::IniFormat)
{
    //auto p = new AudioOutputPrimer();
    //p->start();

    // Construct the UI from the XML
    m_ui->setupUi(this);

    m_ui->statusbar->showMessage(tr("Ready"));

    // Add short cuts to the tool bar.
    m_ui->toolBar->addAction(m_ui->action_open_path);
    m_ui->toolBar->addAction(m_ui->action_undo);
    m_ui->toolBar->addAction(m_ui->action_redo);
    m_ui->toolBar->addAction(m_ui->action_zoom_reset);
    m_ui->toolBar->addAction(m_ui->action_zoom_in);
    m_ui->toolBar->addAction(m_ui->action_zoom_out);

    //connect( m_ui->action_open_path, &QAction::triggered, this, &EditorMainWindow::onOpenPath );
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
            onOpenPath(file);
        }
    }

    m_Settings.setValue("test", 1234);

    int val = m_Settings.value("test").toInt();

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

void EditorMainWindow::onOpenPath(QString fileName)
{
    try
    {
        auto model = std::make_unique<Model>();
        model->LoadJson(fileName.toStdString());

        QFileInfo fileInfo(fileName);

        EditorTab* view = new EditorTab(m_ui->tabWidget, std::move(model), fileName);

        view->setToolTip(fileName);
        const int tabIdx = m_ui->tabWidget->addTab(view, fileInfo.fileName());
        m_ui->tabWidget->setTabToolTip(tabIdx, fileName);
        m_ui->tabWidget->setTabIcon(tabIdx, m_ui->action_open_path->icon());

        m_ui->stackedWidget->setCurrentIndex(1);
    }
    /*
    catch (const IOReadException&)
    {

    }
    catch (const InvalidJsonException&)
    {

    }
    catch (const InvalidGameException&)
    {

    }
    catch (const ObjectPropertyTypeNotFoundException&)
    {

    }
    catch (const JsonKeyNotFoundException&)
    {

    }*/
    catch (const ModelException&)
    {
        QMessageBox::critical(this, "Error", "Failed to load json");
    }
}

void EditorMainWindow::onCloseTab(int index)
{
    auto tab = static_cast<EditorTab*>(m_ui->tabWidget->widget(index));
    if (QMessageBox::question(this, "Confirm", "Close without saving changes?") == QMessageBox::Yes)
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open level"), "", tr("Json files (*.json);;Level files (*.lvl);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        onOpenPath(fileName);
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
