//
// Created by gorev on 28.09.2025.
//

#ifndef EDITOR_SETTINGS_H
#define EDITOR_SETTINGS_H

#include "SettingsData.h"


#include <QDialog>
#include <QPointer>


struct SettingsData;
namespace editor
{
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
    Ui::editor_settings* ui;

    SettingsData data;

private slots:
    void OnSetDirectoryPressed();
};
} // namespace editor

#endif // EDITOR_SETTINGS_H
