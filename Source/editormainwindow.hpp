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

    void on_actionEdit_HintFly_message_triggered();

    void on_actionEdit_LED_messages_triggered();

    void on_actionEdit_path_data_triggered();

    void on_actionEdit_map_size_triggered();

    void on_actionNew_path_triggered();

    void on_actionSave_As_triggered();

    void on_actionAdd_object_triggered();

    void on_actionAdd_collision_triggered();

    void on_action_close_path_triggered();

    void on_actionItem_transparency_triggered();

    void on_action_toggle_show_grid_triggered();

private:
    bool onOpenPath(QString fileName, bool createNewPath);
    void UpdateWindowTitle();
    void DisconnectTabSignals();
    void closeEvent(QCloseEvent* pEvent) override;
private:
    Ui::EditorMainWindow* m_ui;
    QSettings m_Settings;
};
