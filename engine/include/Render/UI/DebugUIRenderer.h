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

    bool ShouldDrawWorldGrid = true;
    bool ShouldDrawGizmo = true;
    bool IsGizmoHovered = false;
    bool UseSnap = false;
    Vec3 SnapValue = {5.0f, 5.0f, 5.0f};

private:
    Input::EventHandle h_keyPressed;
};
}