//
// Created by WhoLeb on 22-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "Render/UI/UIRenderer.h"

#include "imgui.h"
#include "RenderSubsystem.h"
#include "backends/imgui_impl_dx12.h"
#include "Input/InputSubsystem.h"
#include "Input/MouseEvents.h"
#include "Render/CommandQueue.h"

using namespace Blainn;

void UIRenderer::Initialize(int width, int height)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    m_width = width;
    m_height = height;

    io.DisplaySize.x = static_cast<float>(width);
    io.DisplaySize.y = static_cast<float>(height);

    ImGui_ImplDX12_InitInfo initInfo{};
    initInfo.Device = Device::GetInstance().GetDevice2().Get();
    initInfo.CommandQueue = Device::GetInstance().GetCommandQueue()->GetCommandQueue().Get();
    initInfo.NumFramesInFlight = RenderSubsystem::SwapChainFrameCount;
    initInfo.RTVFormat = RenderSubsystem::BackBufferFormat;
    initInfo.DSVFormat = RenderSubsystem::DepthStencilFormat;
    initInfo.SrvDescriptorHeap = Device::GetInstance().GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).Get();
    initInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
    {
        static int32_t descriptorAllocIndex = 2200;
        auto& device = Device::GetInstance();
        auto srvHeap = device.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        auto descriptorSize = device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(srvHeap->GetCPUDescriptorHandleForHeapStart());
        cpuHandle.Offset(descriptorAllocIndex, descriptorSize);

        *out_cpu_handle = cpuHandle;
        *out_gpu_handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(srvHeap->GetGPUDescriptorHandleForHeapStart(), descriptorAllocIndex, descriptorSize);
        descriptorAllocIndex++;
    };
    initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle){};
    ImGui_ImplDX12_Init(&initInfo);


    h_mbPressed = Input::AddEventListener(InputEventType::MouseButtonPressed, [](const InputEventPointer& event)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto mbEvent = static_cast<MouseButtonPressedEvent *>(event.get());

        io.MouseDown[static_cast<int>(mbEvent->GetMouseButton())] = true;
        io.MouseClicked[static_cast<int>(mbEvent->GetMouseButton())] = true;
    });

    h_mbHeld = Input::AddEventListener(InputEventType::MouseButtonHeld, [](const InputEventPointer& event)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto mbEvent = static_cast<MouseButtonPressedEvent *>(event.get());

        io.MouseDown[static_cast<int>(mbEvent->GetMouseButton())] = true;
        io.MouseClicked[static_cast<int>(mbEvent->GetMouseButton())] = false;
    });

    h_mbReleased = Input::AddEventListener(InputEventType::MouseButtonReleased, [](const InputEventPointer& event)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto mbEvent = static_cast<MouseButtonReleasedEvent *>(event.get());

        io.MouseDown[static_cast<uint16_t>(mbEvent->GetMouseButton())] = false;
    });

    m_debugUIRenderer.Initialize();
}

void UIRenderer::Destroy()
{
    m_debugUIRenderer.Destroy();

    Input::RemoveEventListener(InputEventType::MouseButtonPressed, h_mbPressed);
    Input::RemoveEventListener(InputEventType::MouseButtonHeld, h_mbHeld);
    Input::RemoveEventListener(InputEventType::MouseButtonReleased, h_mbReleased);

    ImGui_ImplDX12_Shutdown();
    ImGui::DestroyContext();
}

void UIRenderer::Resize(int width, int height)
{
    ImGuiIO& io = ImGui::GetIO();
    m_width = width; m_height = height;
    io.DisplaySize.x = static_cast<float>(width);
    io.DisplaySize.y = static_cast<float>(height);
}

void UIRenderer::RenderUI(ID3D12GraphicsCommandList2* pCommandList)
{
    ImGui_ImplDX12_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO();

    Input::MousePosition mouse = Input::GetMousePosition();
    io.MousePos = ImVec2(mouse.X, mouse.Y);

    io.KeyCtrl  = Input::IsKeyHeld(KeyCode::LeftCtrl)  || Input::IsKeyHeld(KeyCode::RightCtrl);
    io.KeyShift = Input::IsKeyHeld(KeyCode::LeftShift) || Input::IsKeyHeld(KeyCode::RightShift);
    io.KeyAlt   = Input::IsKeyHeld(KeyCode::Alt);
    io.KeySuper = Input::IsKeyHeld(KeyCode::LeftWin) || Input::IsKeyHeld(KeyCode::RightWin);

    RenderUserUI();
    RenderDebugUI();

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandList);

}

void UIRenderer::RenderUserUI()
{
}

void UIRenderer::RenderDebugUI()
{
    m_debugUIRenderer.DrawDebugUI();
}
