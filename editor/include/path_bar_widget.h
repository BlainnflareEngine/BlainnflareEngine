//
// Created by gorev on 09.10.2025.
//

#pragma once

#include <QHBoxLayout>
#include <QWidget>
#include <qdir.h>


namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class path_bar_widget;
}
QT_END_NAMESPACE

class path_bar_widget : public QWidget
{
    Q_OBJECT

public:
    explicit path_bar_widget(QWidget *parent = nullptr);
    ~path_bar_widget() override;

    void SetRootPath(const QString &root);
    void SetCurrentPath(const QString &path);

signals:
    void PathClicked(const QString &path);

private:
    void UpdatePath();

    Ui::path_bar_widget *m_ui;
    QHBoxLayout *m_layout;

    QString m_rootPath;
    QString m_currentPath;
};
} // namespace editor
