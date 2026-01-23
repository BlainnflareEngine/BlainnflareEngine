//
// Created by WhoLeb on 22-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "Render/UI/UIRenderer.h"

#include "Engine.h"
#include "imgui.h"
#include "RenderSubsystem.h"
#include "backends/imgui_impl_dx12.h"
#include "Input/InputSubsystem.h"
#include "Input/KeyboardEvents.h"
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

    SetupInput();


    m_debugUIRenderer.Initialize();
}

void UIRenderer::Destroy()
{
    m_debugUIRenderer.Destroy();

    Input::RemoveEventListener(InputEventType::MouseMoved, h_mouseMoved);
    Input::RemoveEventListener(InputEventType::MouseButtonPressed, h_mbPressed);
    Input::RemoveEventListener(InputEventType::MouseButtonHeld, h_mbHeld);
    Input::RemoveEventListener(InputEventType::MouseButtonReleased, h_mbReleased);
    Input::RemoveEventListener(InputEventType::MouseScrolled, h_mouseScrolled);

    Input::RemoveEventListener(InputEventType::KeyPressed, h_keyPressed);
    Input::RemoveEventListener(InputEventType::KeyHeld, h_keyHeld);
    Input::RemoveEventListener(InputEventType::KeyReleased, h_keyReleased);

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

void UIRenderer::StartImGuiFrame()
{
    if (m_ImGuiFrameStarted)
        return;
    ImGui_ImplDX12_NewFrame();
    ImGui::NewFrame();
    m_ImGuiFrameStarted = true;
}

void UIRenderer::RenderUI(ID3D12GraphicsCommandList2* pCommandList)
{
    StartImGuiFrame();

    if (ShouldRenderDebugUI)
        RenderDebugUI();

    m_isUIHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || m_debugUIRenderer.IsGizmoHovered();

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandList);
    m_ImGuiFrameStarted = false;
}

void UIRenderer::SetupInput()
{
    h_mouseMoved = Input::AddEventListener(InputEventType::MouseMoved, [](const InputEventPointer& event)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto mbEvent = static_cast<MouseMovedEvent *>(event.get());

        io.AddMousePosEvent((float)mbEvent->GetX(), (float)mbEvent->GetY());
    });

    h_mbPressed = Input::AddEventListener(InputEventType::MouseButtonPressed, [](const InputEventPointer& event)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto mbEvent = static_cast<MouseButtonPressedEvent *>(event.get());

        io.AddMouseButtonEvent((int)mbEvent->GetMouseButton(), true);
    });

    h_mbHeld = Input::AddEventListener(InputEventType::MouseButtonHeld, [](const InputEventPointer& event)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto mbEvent = static_cast<MouseButtonHeldEvent *>(event.get());

        io.AddMouseButtonEvent((int)mbEvent->GetMouseButton(), true);
    });

    h_mbReleased = Input::AddEventListener(InputEventType::MouseButtonReleased, [](const InputEventPointer& event)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto mbEvent = static_cast<MouseButtonReleasedEvent *>(event.get());

        io.AddMouseButtonEvent((int)mbEvent->GetMouseButton(), false);
    });

    h_mouseScrolled = Input::AddEventListener(InputEventType::MouseScrolled, [](const InputEventPointer& event)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto mbEvent = static_cast<MouseScrolledEvent *>(event.get());

        io.AddMouseWheelEvent(mbEvent->GetXOffset() / 120.f, mbEvent->GetYOffset() / 120.f);
    });



    h_keyPressed = Input::AddEventListener(InputEventType::KeyPressed, [](const InputEventPointer& event)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto kEvent = static_cast<KeyPressedEvent *>(event.get());

        auto key = kEvent->GetKey();
        ImGuiKey imGuiKey = static_cast<ImGuiKey>(KeyToImGuiKey(key));
        io.AddKeyEvent(imGuiKey, true);
        });

    h_keyHeld = Input::AddEventListener(InputEventType::KeyHeld, [](const InputEventPointer& event)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto kEvent = static_cast<KeyHeldEvent *>(event.get());

        auto key = kEvent->GetKey();
        ImGuiKey imGuiKey = static_cast<ImGuiKey>(KeyToImGuiKey(key));
        io.AddKeyEvent(imGuiKey, true);
    });

    h_keyReleased = Input::AddEventListener(InputEventType::KeyReleased, [](const InputEventPointer& event)
    {
        ImGuiIO &io = ImGui::GetIO();
        auto kEvent = static_cast<KeyReleasedEvent *>(event.get());

        auto key = kEvent->GetKey();
        ImGuiKey imGuiKey = static_cast<ImGuiKey>(KeyToImGuiKey(key));
        io.AddKeyEvent(imGuiKey, false);
    });
}

constexpr int UIRenderer::KeyToImGuiKey(KeyCode key)
{
    switch (key)
    {
        // ------------------ Core control keys ------------------
        case KeyCode::BackSpace:    return ImGuiKey_Backspace;
        case KeyCode::Tab:          return ImGuiKey_Tab;
        case KeyCode::Enter:        return ImGuiKey_Enter;
        case KeyCode::Escape:       return ImGuiKey_Escape;
        case KeyCode::Space:        return ImGuiKey_Space;

        // ------------------ Arrows & navigation ------------------
        case KeyCode::LeftArrow:    return ImGuiKey_LeftArrow;
        case KeyCode::RightArrow:   return ImGuiKey_RightArrow;
        case KeyCode::UpArrow:      return ImGuiKey_UpArrow;
        case KeyCode::DownArrow:    return ImGuiKey_DownArrow;
        case KeyCode::PageUp:       return ImGuiKey_PageUp;
        case KeyCode::PageDown:     return ImGuiKey_PageDown;
        case KeyCode::Home:         return ImGuiKey_Home;
        case KeyCode::End:          return ImGuiKey_End;

        // ------------------ Edit keys ------------------
        case KeyCode::Insert:        return ImGuiKey_Insert;     // note: typo in your enum → Insert
        case KeyCode::Delete:       return ImGuiKey_Delete;
        case KeyCode::PrintScreen:  return ImGuiKey_PrintScreen;
        case KeyCode::Pause:        return ImGuiKey_Pause;

        // ------------------ Lock keys ------------------
        case KeyCode::CapsLock:     return ImGuiKey_CapsLock;
        case KeyCode::ScrollLock:   return ImGuiKey_ScrollLock;
        case KeyCode::NumLock:      return ImGuiKey_NumLock;

        // ------------------ Main modifiers (both sides map to same logical mod in many cases, but ImGui keeps distinction) ------------------
        case KeyCode::Shift:        // usually logical → you may want Left/Right instead
        case KeyCode::LeftShift:    return ImGuiKey_LeftShift;
        case KeyCode::RightShift:   return ImGuiKey_RightShift;

        case KeyCode::Control:      // logical
        case KeyCode::LeftCtrl:     return ImGuiKey_LeftCtrl;
        case KeyCode::RightCtrl:    return ImGuiKey_RightCtrl;

        case KeyCode::Alt:          // logical
        case KeyCode::LeftMenu:     return ImGuiKey_LeftAlt;
        case KeyCode::RightMenu:    return ImGuiKey_RightAlt;

        case KeyCode::LeftWin:      return ImGuiKey_LeftSuper;
        case KeyCode::RightWin:     return ImGuiKey_RightSuper;

        case KeyCode::Apps:         return ImGuiKey_Menu;

        // ------------------ Letters (A–Z) ------------------
        case KeyCode::A: return ImGuiKey_A;
        case KeyCode::B: return ImGuiKey_B;
        case KeyCode::C: return ImGuiKey_C;
        case KeyCode::D: return ImGuiKey_D;
        case KeyCode::E: return ImGuiKey_E;
        case KeyCode::F: return ImGuiKey_F;
        case KeyCode::G: return ImGuiKey_G;
        case KeyCode::H: return ImGuiKey_H;
        case KeyCode::I: return ImGuiKey_I;
        case KeyCode::J: return ImGuiKey_J;
        case KeyCode::K: return ImGuiKey_K;
        case KeyCode::L: return ImGuiKey_L;
        case KeyCode::M: return ImGuiKey_M;
        case KeyCode::N: return ImGuiKey_N;
        case KeyCode::O: return ImGuiKey_O;
        case KeyCode::P: return ImGuiKey_P;
        case KeyCode::Q: return ImGuiKey_Q;
        case KeyCode::R: return ImGuiKey_R;
        case KeyCode::S: return ImGuiKey_S;
        case KeyCode::T: return ImGuiKey_T;
        case KeyCode::U: return ImGuiKey_U;
        case KeyCode::V: return ImGuiKey_V;
        case KeyCode::W: return ImGuiKey_W;
        case KeyCode::X: return ImGuiKey_X;
        case KeyCode::Y: return ImGuiKey_Y;
        case KeyCode::Z: return ImGuiKey_Z;

        // ------------------ Numbers (top row) ------------------
        case KeyCode::Num0: return ImGuiKey_0;
        case KeyCode::Num1: return ImGuiKey_1;
        case KeyCode::Num2: return ImGuiKey_2;
        case KeyCode::Num3: return ImGuiKey_3;
        case KeyCode::Num4: return ImGuiKey_4;
        case KeyCode::Num5: return ImGuiKey_5;
        case KeyCode::Num6: return ImGuiKey_6;
        case KeyCode::Num7: return ImGuiKey_7;
        case KeyCode::Num8: return ImGuiKey_8;
        case KeyCode::Num9: return ImGuiKey_9;

        // ------------------ Numpad ------------------
        case KeyCode::Numpad0:      return ImGuiKey_Keypad0;
        case KeyCode::Numpad1:      return ImGuiKey_Keypad1;
        case KeyCode::Numpad2:      return ImGuiKey_Keypad2;
        case KeyCode::Numpad3:      return ImGuiKey_Keypad3;
        case KeyCode::Numpad4:      return ImGuiKey_Keypad4;
        case KeyCode::Numpad5:      return ImGuiKey_Keypad5;
        case KeyCode::Numpad6:      return ImGuiKey_Keypad6;
        case KeyCode::Numpad7:      return ImGuiKey_Keypad7;
        case KeyCode::Numpad8:      return ImGuiKey_Keypad8;
        case KeyCode::Numpad9:      return ImGuiKey_Keypad9;

        case KeyCode::Multiply:     return ImGuiKey_KeypadMultiply;
        case KeyCode::Add:          return ImGuiKey_KeypadAdd;
        case KeyCode::Subtract:     return ImGuiKey_KeypadSubtract;
        case KeyCode::Decimal:      return ImGuiKey_KeypadDecimal;
        case KeyCode::Divide:       return ImGuiKey_KeypadDivide;
        case KeyCode::NumpadEqual:  return ImGuiKey_KeypadEqual;
        // Separator usually maps → KeypadEnter on many keyboards
        case KeyCode::Separator:    return ImGuiKey_KeypadEnter;

        // ------------------ Function keys ------------------
        case KeyCode::F1:  return ImGuiKey_F1;
        case KeyCode::F2:  return ImGuiKey_F2;
        case KeyCode::F3:  return ImGuiKey_F3;
        case KeyCode::F4:  return ImGuiKey_F4;
        case KeyCode::F5:  return ImGuiKey_F5;
        case KeyCode::F6:  return ImGuiKey_F6;
        case KeyCode::F7:  return ImGuiKey_F7;
        case KeyCode::F8:  return ImGuiKey_F8;
        case KeyCode::F9:  return ImGuiKey_F9;
        case KeyCode::F10: return ImGuiKey_F10;
        case KeyCode::F11: return ImGuiKey_F11;
        case KeyCode::F12: return ImGuiKey_F12;
        case KeyCode::F13: return ImGuiKey_F13;
        case KeyCode::F14: return ImGuiKey_F14;
        case KeyCode::F15: return ImGuiKey_F15;
        case KeyCode::F16: return ImGuiKey_F16;
        case KeyCode::F17: return ImGuiKey_F17;
        case KeyCode::F18: return ImGuiKey_F18;
        case KeyCode::F19: return ImGuiKey_F19;
        case KeyCode::F20: return ImGuiKey_F20;
        case KeyCode::F21: return ImGuiKey_F21;
        case KeyCode::F22: return ImGuiKey_F22;
        case KeyCode::F23: return ImGuiKey_F23;
        case KeyCode::F24: return ImGuiKey_F24;

        // ------------------ Punctuation (US layout assumed) ------------------
        //case KeyCode::Apostrophe:   // ' "
        //    return ImGuiKey_Apostrophe;
        case KeyCode::Comma:        return ImGuiKey_Comma;
        case KeyCode::Minus:        return ImGuiKey_Minus;
        case KeyCode::Period:       return ImGuiKey_Period;
        case KeyCode::OEM2:         // ? / (US)
            return ImGuiKey_Slash;
        case KeyCode::OEM1:         // ; : (US)
            return ImGuiKey_Semicolon;
        case KeyCode::Plus:         // = + (US)
            return ImGuiKey_Equal;
        case KeyCode::OEM4:         // [ { (US)
            return ImGuiKey_LeftBracket;
        case KeyCode::OEM5:         // \ | (US)
            return ImGuiKey_Backslash;
        case KeyCode::OEM6:         // ] } (US)
            return ImGuiKey_RightBracket;
        case KeyCode::OEM3:         // ` ~ (US)
            return ImGuiKey_GraveAccent;

        // ------------------ Browser / Media ------------------
        case KeyCode::BrowserBack:    return ImGuiKey_AppBack;
        case KeyCode::BrowserForward: return ImGuiKey_AppForward;
        // Others (Refresh, Stop, Search, Favorites…) → usually no direct ImGuiKey

        // ------------------ Gamepad ------------------
        case KeyCode::Gamepad_A:               return ImGuiKey_GamepadFaceDown;
        case KeyCode::Gamepad_B:               return ImGuiKey_GamepadFaceRight;
        case KeyCode::Gamepad_X:               return ImGuiKey_GamepadFaceLeft;
        case KeyCode::Gamepad_Y:               return ImGuiKey_GamepadFaceUp;
        case KeyCode::GamepadLeftBumper:       return ImGuiKey_GamepadL1;
        case KeyCode::GamepadRightBumper:      return ImGuiKey_GamepadR1;
        case KeyCode::GamepadLeftTrigger:      return ImGuiKey_GamepadL2;
        case KeyCode::GamepadRightTrigger:     return ImGuiKey_GamepadR2;
        case KeyCode::GamepadLeftStickBtn:     return ImGuiKey_GamepadL3;
        case KeyCode::GamepadRightStickBtn:    return ImGuiKey_GamepadR3;
        case KeyCode::GamepadDPadUp:           return ImGuiKey_GamepadDpadUp;
        case KeyCode::GamepadDPadDown:         return ImGuiKey_GamepadDpadDown;
        case KeyCode::GamepadDPadLeft:         return ImGuiKey_GamepadDpadLeft;
        case KeyCode::GamepadDPadRight:        return ImGuiKey_GamepadDpadRight;
        case KeyCode::GamepadMenu:             return ImGuiKey_GamepadStart;
        case KeyCode::GamepadView:             return ImGuiKey_GamepadBack;
        case KeyCode::GamepadLeftStickUp:      return ImGuiKey_GamepadLStickUp;
        case KeyCode::GamepadLeftStickDown:    return ImGuiKey_GamepadLStickDown;
        case KeyCode::GamepadLeftStickLeft:    return ImGuiKey_GamepadLStickLeft;
        case KeyCode::GamepadLeftStickRight:   return ImGuiKey_GamepadLStickRight;
        // Right stick analogs → no perfect 1:1 match, but closest:
        case KeyCode::GamepadRightStickUp:     return ImGuiKey_GamepadRStickUp;
        case KeyCode::GamepadRightStickDown:   return ImGuiKey_GamepadRStickDown;
        case KeyCode::GamepadRightStickLeft:   return ImGuiKey_GamepadRStickLeft;
        case KeyCode::GamepadRightStickRight:  return ImGuiKey_GamepadRStickRight;

        // ------------------ Keys without direct match ------------------
        // You can decide whether to return ImGuiKey_None or pick a "closest" key
        case KeyCode::CtrlBrkPrcs:
        case KeyCode::Clear:
        case KeyCode::Kana:     //case KeyCode::Hangeul:  case KeyCode::Hangul:
        case KeyCode::Junju:    case KeyCode::Final:
        case KeyCode::Hanja:    //case KeyCode::Kanji:
        case KeyCode::Convert:  case KeyCode::NonConvert:
        case KeyCode::Accept:   case KeyCode::ModeChange:
        case KeyCode::Select:
        case KeyCode::Print:
        case KeyCode::Execute:
        case KeyCode::Help:
        case KeyCode::Sleep:
        case KeyCode::VolumeMute:
        case KeyCode::VolumeDown:
        case KeyCode::VolumeUp:
        case KeyCode::NextTrack:
        case KeyCode::PrevTrack:
        case KeyCode::Stop:
        case KeyCode::PlayPause:
        case KeyCode::OEM102:           return ImGuiKey_Oem102;  // best match (extra \ key on EU/ISO kbds)
        case KeyCode::OEM7:             return ImGuiKey_Apostrophe; // sometimes closest
        case KeyCode::OEM8:
        case KeyCode::OEMAX:
        case KeyCode::Packet:
        case KeyCode::OEMReset:   case KeyCode::OEMJump:
        case KeyCode::OEMPA1:     case KeyCode::OEMPA2:   case KeyCode::OEMPA3:
        case KeyCode::OEMWSCtrl:  case KeyCode::OEMCusel: case KeyCode::OEMAttn:
        case KeyCode::OEMFinish:  case KeyCode::OEMCopy:  case KeyCode::OEMAuto:
        case KeyCode::OEMEnlw:    case KeyCode::OEMBackTab:
        case KeyCode::Attn:       case KeyCode::CrSel:    case KeyCode::ExSel:
        case KeyCode::EraseEOF:   case KeyCode::Play:     case KeyCode::Zoom:
        case KeyCode::NoName:     case KeyCode::PA1:      case KeyCode::OEMClear:
            return ImGuiKey_None;

        default:
            return ImGuiKey_None;
    }
}

void UIRenderer::RenderDebugUI()
{
    m_debugUIRenderer.DrawDebugUI();
}
