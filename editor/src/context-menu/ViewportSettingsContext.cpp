//
// Created by gorev on 22.01.2026.
//

#include "context-menu/ViewportSettingsContext.h"

#include "Editor.h"
#include "Engine.h"
#include "RenderSubsystem.h"
#include "Render/UI/UIRenderer.h"
#include "input-widgets/float_input_field.h"

#include <QToolButton>
#include <QMenu>
#include <QToolBar>
#include <QWidgetAction>

namespace editor
{

ViewportSettingsContext::ViewportSettingsContext(QToolButton *toolButton, QWidget *parent)
    : QWidget(parent)
    , m_toolButton(toolButton)
{
}


ViewportSettingsContext::~ViewportSettingsContext()
{
    SaveValues();
}


void ViewportSettingsContext::Initialize()
{
    RestoreValues();

    m_viewportSettingsMenu = new QMenu(m_toolButton);

    auto &render = Blainn::RenderSubsystem::GetInstance();

    QAction *vsyncAction = m_viewportSettingsMenu->addAction("VSync");
    vsyncAction->setCheckable(true);
    vsyncAction->setChecked(render.GetVSyncEnabled());

    QAction *enablePickingAction = m_viewportSettingsMenu->addAction("Enable picking");
    enablePickingAction->setCheckable(true);
    enablePickingAction->setChecked(Blainn::Engine::GetSelectionManager().EnablePicking);

    QAction *debugPhysicsAction = m_viewportSettingsMenu->addAction("Debug lines");
    debugPhysicsAction->setCheckable(true);
    debugPhysicsAction->setChecked(render.DebugEnabled());

    QAction *renderDebugUIAction = m_viewportSettingsMenu->addAction("Render debug ui");
    renderDebugUIAction->setCheckable(true);
    renderDebugUIAction->setChecked(render.GetUIRenderer().ShouldRenderDebugUI);

    QAction *frameTimeAction = m_viewportSettingsMenu->addAction("Show frame time");
    frameTimeAction->setCheckable(true);
    frameTimeAction->setChecked(render.GetUIRenderer().GetDebugUIRenderer().ShouldDrawFrameTime);

    QAction *gridAction = m_viewportSettingsMenu->addAction("Show world grid");
    gridAction->setCheckable(true);
    gridAction->setChecked(render.GetUIRenderer().GetDebugUIRenderer().ShouldDrawWorldGrid);

    QAction *gizmoAction = m_viewportSettingsMenu->addAction("Show gizmo");
    gizmoAction->setCheckable(true);
    gizmoAction->setChecked(render.GetUIRenderer().GetDebugUIRenderer().ShouldDrawGizmo);

    QAction *setGizmoModeAction = m_viewportSettingsMenu->addAction("World space gizmo");
    setGizmoModeAction->setCheckable(true);
    setGizmoModeAction->setChecked(render.GetUIRenderer().GetDebugUIRenderer().WorldGizmo);

    QAction *snapAction = m_viewportSettingsMenu->addAction("Use snapping");
    snapAction->setCheckable(true);
    snapAction->setChecked(render.GetUIRenderer().GetDebugUIRenderer().UseSnap);

    float_input_field *translationField =
        new float_input_field("Translation snapping", render.GetUIRenderer().GetDebugUIRenderer().TranslationSnapValue);
    translationField->SetMinValue(0.00);
    translationField->SetMaxValue(100);
    translationField->SetDecimals(2);
    translationField->SetSingleStep(0.5);

    auto translationAction = new QWidgetAction(m_viewportSettingsMenu);
    translationAction->setDefaultWidget(translationField);
    m_viewportSettingsMenu->addAction(translationAction);

    float_input_field *rotationField =
        new float_input_field("Rotation snapping", render.GetUIRenderer().GetDebugUIRenderer().RotationSnapValue);
    rotationField->SetMinValue(0.00);
    rotationField->SetMaxValue(100);
    rotationField->SetDecimals(2);
    rotationField->SetSingleStep(0.5);

    auto rotationAction = new QWidgetAction(m_viewportSettingsMenu);
    rotationAction->setDefaultWidget(rotationField);
    m_viewportSettingsMenu->addAction(rotationAction);

    float_input_field *scaleField =
        new float_input_field("Scale snapping", render.GetUIRenderer().GetDebugUIRenderer().ScaleSnapValue);
    scaleField->SetMinValue(0.00);
    scaleField->SetMaxValue(100);
    scaleField->SetDecimals(2);
    scaleField->SetSingleStep(0.5);

    auto scaleAction = new QWidgetAction(m_viewportSettingsMenu);
    scaleAction->setDefaultWidget(scaleField);
    m_viewportSettingsMenu->addAction(scaleAction);

    connect(vsyncAction, &QAction::toggled, this, &ViewportSettingsContext::EnableVSync);
    connect(enablePickingAction, &QAction::toggled, this, &ViewportSettingsContext::EnablePicking);
    connect(debugPhysicsAction, &QAction::toggled, this, &ViewportSettingsContext::ShowDebugLines);
    connect(renderDebugUIAction, &QAction::toggled, this, &ViewportSettingsContext::RenderDebugUI);
    connect(frameTimeAction, &QAction::toggled, this, &ViewportSettingsContext::ShowFrameTime);
    connect(gridAction, &QAction::toggled, this, &ViewportSettingsContext::EnableWorldGrid);
    connect(gizmoAction, &QAction::toggled, this, &ViewportSettingsContext::EnableGizmo);
    connect(setGizmoModeAction, &QAction::toggled, this, &ViewportSettingsContext::SetGizmoMode);
    connect(snapAction, &QAction::toggled, this, &ViewportSettingsContext::UseSnapping);

    connect(translationField, &float_input_field::EditingFinished, this,
            [this, translationField]() { OnTranslationSnappingChanged(translationField->GetValue()); });
    connect(rotationField, &float_input_field::EditingFinished, this,
            [this, rotationField]() { OnRotationSnappingChanged(rotationField->GetValue()); });
    connect(scaleField, &float_input_field::EditingFinished, this,
            [this, scaleField]() { OnScaleSnappingChanged(scaleField->GetValue()); });


    m_toolButton->setCheckable(true);
    connect(m_toolButton, &QToolButton::toggled, this, &ViewportSettingsContext::ShowMenu);
    connect(m_viewportSettingsMenu, &QMenu::aboutToHide, m_toolButton, [this]()
    {
        m_viewportSettingsMenu->clearFocus();
        m_toolButton->clearFocus();
        m_toolButton->setChecked(false);
    });
}


void ViewportSettingsContext::SaveValues()
{
    if (auto config = Blainn::Editor::GetInstance().GetEditorConfig())
    {
        auto &render = Blainn::RenderSubsystem::GetInstance();
        auto &debugUIRenderer = render.GetUIRenderer().GetDebugUIRenderer();

        config["VSync"] = render.GetVSyncEnabled();
        config["EnablePicking"] = Blainn::Engine::GetSelectionManager().EnablePicking;
        config["DebugLines"] = render.DebugEnabled();
        config["ShouldRenderDebugUI"] = render.GetUIRenderer().ShouldRenderDebugUI;
        config["ShouldDrawFrameTime"] = debugUIRenderer.ShouldDrawFrameTime;
        config["ShouldDrawWorldGrid"] = debugUIRenderer.ShouldDrawWorldGrid;
        config["ShouldDrawGizmo"] = debugUIRenderer.ShouldDrawGizmo;
        config["GizmoWorldMode"] = debugUIRenderer.WorldGizmo;
        config["UseSnap"] = debugUIRenderer.UseSnap;
        config["TranslationSnapValue"] = debugUIRenderer.TranslationSnapValue;
        config["RotationSnapValue"] = debugUIRenderer.RotationSnapValue;
        config["ScaleSnapValue"] = debugUIRenderer.ScaleSnapValue;

        std::ofstream fout(Blainn::Editor::GetInstance().GetEditorConfigPath().string());
        fout << config;
    }
}


void ViewportSettingsContext::RestoreValues()
{
    if (auto config = Blainn::Editor::GetInstance().GetEditorConfig())
    {
        auto &render = Blainn::RenderSubsystem::GetInstance();
        auto &debugUIRenderer = render.GetUIRenderer().GetDebugUIRenderer();

        if (config["VSync"]) render.SetVSyncEnabled(config["VSync"].as<bool>());
        if (config["EnablePicking"])
            Blainn::Engine::GetSelectionManager().EnablePicking = config["EnablePicking"].as<bool>();
        if (config["DebugLines"]) render.SetEnableDebug(config["DebugLines"].as<bool>());
        if (config["ShouldRenderDebugUI"])
            render.GetUIRenderer().ShouldRenderDebugUI = config["ShouldRenderDebugUI"].as<bool>();
        if (config["ShouldDrawFrameTime"])
            debugUIRenderer.ShouldDrawFrameTime = config["ShouldDrawFrameTime"].as<bool>();
        if (config["ShouldDrawWorldGrid"])
            debugUIRenderer.ShouldDrawWorldGrid = config["ShouldDrawWorldGrid"].as<bool>();
        if (config["ShouldDrawGizmo"]) debugUIRenderer.ShouldDrawGizmo = config["ShouldDrawGizmo"].as<bool>();
        if (config["GizmoWorldMode"]) debugUIRenderer.WorldGizmo = config["GizmoWorldMode"].as<bool>();
        if (config["UseSnap"]) debugUIRenderer.UseSnap = config["UseSnap"].as<bool>();
        if (config["TranslationSnapValue"])
            debugUIRenderer.TranslationSnapValue = config["TranslationSnapValue"].as<float>();
        if (config["RotationSnapValue"]) debugUIRenderer.RotationSnapValue = config["RotationSnapValue"].as<float>();
        if (config["ScaleSnapValue"]) debugUIRenderer.ScaleSnapValue = config["ScaleSnapValue"].as<float>();
    }
}


void ViewportSettingsContext::ShowMenu(bool checked)
{
    if (checked)
    {
        QPoint pos = m_toolButton->mapToGlobal(m_toolButton->rect().bottomLeft());
        m_viewportSettingsMenu->popup(pos);
    }
    else
    {
        m_viewportSettingsMenu->close();
    }
}


void ViewportSettingsContext::EnableVSync(bool value)
{
    Blainn::RenderSubsystem::GetInstance().SetVSyncEnabled(value);
}


void ViewportSettingsContext::EnablePicking(bool value)
{
    Blainn::Engine::GetSelectionManager().EnablePicking = value;
}


void ViewportSettingsContext::ShowDebugLines(bool value)
{
    Blainn::RenderSubsystem::GetInstance().SetEnableDebug(value);
}

void ViewportSettingsContext::RenderDebugUI(bool value)
{
    Blainn::RenderSubsystem::GetInstance().GetUIRenderer().ShouldRenderDebugUI = value;
}

void ViewportSettingsContext::ShowFrameTime(bool value)
{
    Blainn::RenderSubsystem::GetInstance().GetUIRenderer().GetDebugUIRenderer().ShouldDrawFrameTime = value;
}


void ViewportSettingsContext::EnableWorldGrid(bool value)
{
    Blainn::RenderSubsystem::GetInstance().GetUIRenderer().GetDebugUIRenderer().ShouldDrawWorldGrid = value;
}


void ViewportSettingsContext::EnableGizmo(bool value)
{
    Blainn::RenderSubsystem::GetInstance().GetUIRenderer().GetDebugUIRenderer().ShouldDrawGizmo = value;
}


void ViewportSettingsContext::SetGizmoMode(bool value)
{
    Blainn::RenderSubsystem::GetInstance().GetUIRenderer().GetDebugUIRenderer().WorldGizmo = value;
}


void ViewportSettingsContext::UseSnapping(bool value)
{
    Blainn::RenderSubsystem::GetInstance().GetUIRenderer().GetDebugUIRenderer().UseSnap = value;
}


void ViewportSettingsContext::OnTranslationSnappingChanged(int value)
{
    Blainn::RenderSubsystem::GetInstance().GetUIRenderer().GetDebugUIRenderer().TranslationSnapValue = value;
}


void ViewportSettingsContext::OnRotationSnappingChanged(int value)
{
    Blainn::RenderSubsystem::GetInstance().GetUIRenderer().GetDebugUIRenderer().RotationSnapValue = value;
}


void ViewportSettingsContext::OnScaleSnappingChanged(int value)
{
    Blainn::RenderSubsystem::GetInstance().GetUIRenderer().GetDebugUIRenderer().ScaleSnapValue = value;
}
} // namespace editor