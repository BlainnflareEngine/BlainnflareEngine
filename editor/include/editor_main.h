//
// Created by gorev on 21.09.2025.
//

#pragma once

#include "context-menu/AddToSceneContextMenu.h"
#include "inspector_widget.h"


#include <QMainWindow>
#include <qdir.h>

namespace editor
{
class console_messages_widget;
}
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


    void SetContentDirectory(const QString &path);
    void closeEvent(QCloseEvent *event) override;

    HWND GetViewportHWND() const;
    inspector_widget &GetInspectorWidget();
    console_messages_widget *GetConsoleWidget() const;

private:
    Ui::editor_main *ui;

    QString m_contentPath;

    void OpenAddToScene() const;

private slots:
    void OnOpenSettings();
    void OnSaveScene();

    void OnBuildNavMesh();

    void OnStartPlayMode();
    void OnStopPlayMode();
};

} // namespace editor
