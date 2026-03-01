//
// Created by WhoLeb on 22-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include "DebugUIRenderer.h"
#include "Input/InputSubsystem.h"
#include "d3d12.h"

namespace Blainn
{
class UIRenderer
{
public:
    UIRenderer() = default;
    ~UIRenderer() = default;

    void Initialize(int width, int height);
    void Destroy();

    void Resize(int width, int height);

    void StartImGuiFrame();
    void RenderUI(ID3D12GraphicsCommandList2* pCommandList);

    DebugUIRenderer &GetDebugUIRenderer()
    {
        return m_debugUIRenderer;
    }

    bool IsUIHovered() const {return m_isUIHovered;}

public:
    bool ShouldRenderDebugUI = true;

private:

    void RenderDebugUI();

    void SetupInput();
    static constexpr int KeyToImGuiKey(KeyCode key);

private:
    DebugUIRenderer m_debugUIRenderer;
    int m_width, m_height;

    bool m_ImGuiFrameStarted = false;
    bool m_isUIHovered = false;

    Input::EventHandle h_mouseMoved;
    Input::EventHandle h_mbPressed;
    Input::EventHandle h_mbHeld;
    Input::EventHandle h_mbReleased;
    Input::EventHandle h_mouseScrolled;

    Input::EventHandle h_keyPressed;
    Input::EventHandle h_keyHeld;
    Input::EventHandle h_keyReleased;
};

}