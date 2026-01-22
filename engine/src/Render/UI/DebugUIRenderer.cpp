//
// Created by WhoLeb on 22-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "Render/UI/DebugUIRenderer.h"

#include "Engine.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "RenderSubsystem.h"
#include "Input/KeyboardEvents.h"

using namespace Blainn;


void DebugUIRenderer::Initialize()
{
    h_keyPressed = Input::AddEventListener(InputEventType::KeyPressed, [this](const InputEventPointer& e)
    {
        auto ke = static_cast<KeyPressedEvent*>(e.get());
        switch(ke->GetKey())
        {
        case Key::E:
            SelectedGizmoMode = GizmoOperationModes::TRANSLATE;
            break;
        case Key::R:
            SelectedGizmoMode = GizmoOperationModes::ROTATE;
            break;
        case Key::T:
            SelectedGizmoMode = GizmoOperationModes::SCALE;
            break;
        case Key::Y:
            UseSnap = !UseSnap;
            break;
        default:
            break;
        }
    });
}

void DebugUIRenderer::Destroy()
{
    Input::RemoveEventListener(InputEventType::KeyPressed, h_keyPressed);
    ShouldDrawGizmo = false;
    ShouldDrawWorldGrid = false;
    IsGizmoHovered = false;
}

void DebugUIRenderer::DrawDebugUI()
{
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

    auto& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    DrawWorldGrid();
    DrawGizmo();
}

void DebugUIRenderer::DrawWorldGrid()
{
    if (ShouldDrawWorldGrid)
    {
        auto camera = RenderSubsystem::GetInstance().GetCamera();
        Mat4 cameraView = camera->GetViewMatrix();
        Mat4 cameraProjection = camera->GetPerspectiveProjectionMatrix();

        /*
        Vec3 camPos = camera->GetPosition();
        Vec3 translation = {
            round(camPos.x),
            0,
            round(camPos.z)
        };
        matrix.Translation(translation);
        */
        Mat4 matrix = Mat4::Identity;

        ImGuizmo::DrawGrid(
            reinterpret_cast<float*>(&cameraView),
            reinterpret_cast<float*>(&cameraProjection),
            reinterpret_cast<float*>(&matrix),
            100
            );
    }
}

void DebugUIRenderer::DrawGizmo()
{
    IsGizmoHovered = false;
    auto selectedUuid = Engine::GetSelectionManager().GetSelectedUUID();
    Entity selectedEntity = Engine::GetActiveScene()->TryGetEntityWithUUID(selectedUuid);

    if (ShouldDrawGizmo && selectedEntity.IsValid() && selectedEntity.HasComponent<TransformComponent>())
    {
        ImGuizmo::Enable(true);

        //static ImGuizmo::OPERATION mCurrentGizmoOperation(static_cast<ImGuizmo::OPERATION>(ImGuizmo::UNIVERSAL ^ ImGuizmo::ROTATE_SCREEN));
        ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;//(ImGuizmo::ROTATE_X | ImGuizmo::ROTATE_Y | ImGuizmo::ROTATE_Z);
        switch (SelectedGizmoMode)
        {
        case GizmoOperationModes::TRANSLATE:
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            break;
        case GizmoOperationModes::ROTATE:
            mCurrentGizmoOperation = ImGuizmo::ROTATE_X | ImGuizmo::ROTATE_Y | ImGuizmo::ROTATE_Z;
            break;
        case GizmoOperationModes::SCALE:
            mCurrentGizmoOperation = ImGuizmo::SCALE;
            break;
        }
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

        auto camera = RenderSubsystem::GetInstance().GetCamera();
        Mat4 cameraView = camera->GetViewMatrix();
        Mat4 cameraProjection = camera->GetPerspectiveProjectionMatrix();
        Mat4 matrix = Engine::GetActiveScene()->GetWorldSpaceTransformMatrix(selectedEntity);
        ImGuizmo::Manipulate(
            reinterpret_cast<float*>(&cameraView.m),
            reinterpret_cast<float*>(&cameraProjection.m),
            mCurrentGizmoOperation,
            mCurrentGizmoMode,
            reinterpret_cast<float*>(&matrix.m),
            NULL,
            UseSnap ? reinterpret_cast<float*>(&SnapValue) : nullptr
            );
        Engine::GetActiveScene()->SetFromWorldSpaceTransformMatrix(selectedEntity, matrix);

        IsGizmoHovered = ImGuizmo::IsOver();
    }
}

void DebugUIRenderer::DrawDebugPanels()
{
}



