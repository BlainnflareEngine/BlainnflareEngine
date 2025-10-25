//
// Created by gorev on 21.09.2025.
//

#ifndef EDITOR_MAIN_H
#define EDITOR_MAIN_H

#include "../../libs/EASTL/include/EASTL/unique_ptr.h"
#include "inspector_widget.h"


#include <QMainWindow>
#include <QPointer>
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

    HWND GetViewportHWND() const;

    void SetContentDirectory(const QString &path);

    void closeEvent(QCloseEvent *event) override;

    inspector_widget& GetInspectorWidget();

    console_messages_widget *GetConsoleWidget() const;

private:
    Ui::editor_main *ui;

    QString m_contentPath;

private slots:
    void OnOpenSettings();
};

} // namespace editor

#endif // EDITOR_MAIN_H
