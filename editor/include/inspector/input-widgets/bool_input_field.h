//
// Created by gorev on 29.11.2025.
//

#pragma once
#include <QCheckBox>


class QLabel;
namespace editor
{

class bool_input_field : public QWidget
{
    Q_OBJECT

public:
    explicit bool_input_field(const QString &name, bool defaultValue = false, QWidget *parent = nullptr,
                              const QColor &nameColor = QColor(255, 255, 255, 255));

    bool isChecked() const;
    void setChecked(bool checked);

signals:
    void toggled(bool checked);

private:
    QCheckBox *m_checkbox;
    QLabel *m_label;
};

} // namespace editor