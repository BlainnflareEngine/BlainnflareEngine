//
// Created by gorev on 14.10.2025.
//

#pragma once

#include <QWidget>

namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class inspector_content_base;
}
QT_END_NAMESPACE

class inspector_content_base : public QWidget
{
    Q_OBJECT

public:
    explicit inspector_content_base(QWidget *parent = nullptr);
    ~inspector_content_base() override;

private:
    Ui::inspector_content_base *ui;
};
} // namespace editor

