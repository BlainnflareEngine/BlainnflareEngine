//
// Created by gorev on 11.12.2025.
//

#pragma once


#include <QWidget>


class QPushButton;
class QLabel;
namespace editor
{

class color_input_field : public QWidget
{
    Q_OBJECT

public:
    explicit color_input_field(const QString &name, QColor value = QColor(255, 255, 255, 255),
                               QWidget *parent = nullptr);

    void SetValue(const QColor &value);
    QColor &GetValue();

    bool HasFocus() const;

signals:
    void EditingFinished();

private slots:
    void OnColorButtonClicked();

private:
    QLabel *m_label = nullptr;
    QPushButton *m_button = nullptr;
    QColor m_color;
};

} // namespace editor