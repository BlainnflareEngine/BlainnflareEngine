//
// Created by gorev on 22.01.2026.
//

#pragma once
#include "oclero/qlementine/widgets/AbstractItemListWidget.hpp"


class QMenu;
class QToolButton;
namespace editor
{
class ViewportSettingsContext : public QWidget
{
    Q_OBJECT
public:
    ViewportSettingsContext(QToolButton *toolButton, QWidget *parent = nullptr);

    ~ViewportSettingsContext();

    void Initialize();

    void SaveValues();
    void RestoreValues();

    void ShowMenu();

protected:
    void ShowDebugLines(bool value);
    void EnableVSync(bool value);
    void EnableGizmo(bool value);
    void EnableWorldGrid(bool value);
    void UseSnapping(bool value);

    void OnTranslationSnappingChanged(int value);
    void OnRotationSnappingChanged(int value);
    void OnScaleSnappingChanged(int value);

private:
    QToolButton *m_toolButton = nullptr;
    QMenu *m_viewportSettingsMenu = nullptr;
};
} // namespace editor