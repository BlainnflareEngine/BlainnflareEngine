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

    ~ViewportSettingsContext() noexcept;

    void Initialize();

    void SaveValues();
    void RestoreValues();

    void ShowMenu(bool checked);

protected:
    void EnablePicking(bool value);
    void EnableVSync(bool value);
    void ShowDebugLines(bool value);
    void RenderDebugUI(bool value);
    void ShowFrameTime(bool value);
    void EnableGizmo(bool value);
    void SetGizmoMode(bool value);
    void EnableWorldGrid(bool value);
    void UseSnapping(bool value);

    void OnTranslationSnappingChanged(float value);
    void OnRotationSnappingChanged(float value);
    void OnScaleSnappingChanged(float value);

private:
    QToolButton *m_toolButton = nullptr;
    QMenu *m_viewportSettingsMenu = nullptr;
};
} // namespace editor
