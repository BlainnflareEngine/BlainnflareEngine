//
// Created by WhoLeb on 22-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include "Input/InputSubsystem.h"

namespace Blainn
{
class DebugUIRenderer
{
public:
    DebugUIRenderer() = default;

    void Initialize();
    void Destroy();

    void DrawDebugUI();

    bool IsGizmoHovered() const { return m_isGizmoHovered; }

private:
    void DrawWorldGrid();
    void DrawGizmo();
    void DrawDebugPanels();

public:
    enum GizmoOperationModes
    {
        TRANSLATE,
        ROTATE,
        SCALE
    } SelectedGizmoMode = GizmoOperationModes::TRANSLATE;

    bool ShouldDrawFrameTime = false;
    bool ShouldDrawWorldGrid = true;
    bool ShouldDrawGizmo = true;
    bool UseSnap = false;
    float TranslationSnapValue = 0.5f;
    float RotationSnapValue = 5.0f;
    float ScaleSnapValue = 0.1f;

private:
    Input::EventHandle h_keyPressed;
    bool m_isGizmoHovered = false;
};
}