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

    void RenderUI(ID3D12GraphicsCommandList2* pCommandList);

    DebugUIRenderer &GetDebugUIRenderer()
    {
        return m_debugUIRenderer;
    }

private:
    void RenderUserUI();
    void RenderDebugUI();

private:
    DebugUIRenderer m_debugUIRenderer;
    int m_width, m_height;

    Input::EventHandle h_mbPressed;
    Input::EventHandle h_mbHeld;
    Input::EventHandle h_mbReleased;
};
}