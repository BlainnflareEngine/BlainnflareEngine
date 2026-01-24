//
// Created by gorev on 14.10.2025.
//

#pragma once


#include <QScrollArea>
#include <QVBoxLayout>


namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class inspector_widget;
}
QT_END_NAMESPACE

class inspector_widget : public QScrollArea
{
    Q_OBJECT

public:
    explicit inspector_widget(QWidget *parent = nullptr);
    ~inspector_widget() override;

    void SetItem(QWidget *item);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    bool IsLocked() const;
    void SetLocked(bool locked);

private:
    Ui::inspector_widget *ui;

    bool m_locked = false;
};
} // namespace editor
