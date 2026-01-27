//
// Created by gorev on 21.09.2025.
//

#pragma once

#include "context-menu/SceneContextMenu.h"
#include "inspector_widget.h"
#include "scene/Scene.h"


#include <QMainWindow>
#include <qdir.h>
#include <QProcess>


namespace editor
{
class ViewportSettingsContext;
class console_messages_widget;
} // namespace editor
namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class editor_main;
}
QT_END_NAMESPACE

class editor_main : public QMainWindow
{
    Q_OBJECT

public:
    explicit editor_main(QWidget *parent = nullptr);
    ~editor_main() override;

    void PostInit();

    void SetContentDirectory(const QString &path);
    void closeEvent(QCloseEvent *event) override;

    HWND GetViewportHWND() const;
    inspector_widget &GetInspectorWidget();
    console_messages_widget *GetConsoleWidget() const;

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::editor_main *ui;

    inspector_widget *m_inspector = nullptr;
    QString m_contentPath;
    ViewportSettingsContext *m_viewportSettingsContext;

    void OpenAddToScene() const;

    eastl::vector<eastl::pair<Blainn::Scene::EventHandle, Blainn::SceneEventType>> m_sceneEvents;

    QProcess *m_buildProcess = nullptr;

private slots:
    void OnOpenSettings();
    void OnSaveScene();

    void OnBuildNavMesh();

    void OnStartPlayMode();
    void OnStopPlayModeToggle();

    void StartGameBuild();

    void OnBuildFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void OnBuildOutputAvailable();
    void OnBuildErrorAvailable();
};

} // namespace editor
