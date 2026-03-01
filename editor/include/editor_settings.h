//
// Created by gorev on 28.09.2025.
//

#pragma once

#include "SettingsData.h"


#include <QDialog>


struct SettingsData;
namespace editor
{
class path_input_field;
QT_BEGIN_NAMESPACE
namespace Ui
{
class editor_settings;
}
QT_END_NAMESPACE

class editor_settings : public QDialog
{
    Q_OBJECT

public:
    explicit editor_settings(const SettingsData &data, QWidget *parent = nullptr);
    ~editor_settings() override;

private:
    Ui::editor_settings *ui;

    SettingsData data;

    path_input_field *m_defaultSceneInput = nullptr;

private slots:
    void OnDefaultScenePathChanged(const QString &newPath);
};
} // namespace editor
