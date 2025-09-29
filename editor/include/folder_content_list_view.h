﻿//
// Created by gorev on 24.09.2025.
//

#pragma once


#include <QListView>
#include <QPointer>

namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class folder_content_list_view;
}
QT_END_NAMESPACE

class folder_content_list_view : public QListView
{
    Q_OBJECT

public:
    explicit folder_content_list_view(QWidget *parent = nullptr);
    ~folder_content_list_view() override;

protected:
    void dropEvent(QDropEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;

    void dragMoveEvent(QDragMoveEvent *event) override;

private:
    Ui::folder_content_list_view* ui;
};
} // namespace editor
