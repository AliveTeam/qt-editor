#pragma once

#include <QMainWindow>
#include <memory>
#include <QSettings>

namespace Ui
{
    class EditorMainWindow;
}

class EditorMainWindow final : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit EditorMainWindow( QWidget* aParent = 0 );
    ~EditorMainWindow();
private slots:
    void on_action_about_qt_triggered();

    void on_action_about_triggered();

    void onCloseTab(int index);

    void on_action_open_path_triggered();

    void on_action_zoom_reset_triggered();

    void on_action_zoom_in_triggered();

    void on_action_zoom_out_triggered();

    void on_action_undo_triggered();

    void on_action_redo_triggered();

    void on_action_save_path_triggered();

    void on_actionExport_to_lvl_triggered();

    void on_tabWidget_currentChanged(int index);

private:
    bool onOpenPath(QString fileName);
    void UpdateWindowTitle(bool clean);
    void DisconnectTabSignals();
    void closeEvent(QCloseEvent* pEvent) override;
private:
    Ui::EditorMainWindow* m_ui;
    QSettings m_Settings;
};
