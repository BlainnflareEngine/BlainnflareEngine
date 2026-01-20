//
// Created by WhoLeb on 06-Nov-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#pragma once
#include "aliases.h"

namespace Blainn
{
struct TransformComponent
{
private:
    inline static const int kNumFramesMarkDirty = 3;

    // dirty flag available between frames
    int NumFramesDirty = kNumFramesMarkDirty; // NumFrameResources

    Vec3 Translation{0.f, 0.f, 0.f};
    Vec3 Scale{1.f, 1.f, 1.f};

    // Euler is stored as Yaw Pitch Roll
    Vec3 EulerRotation{0.f, 0.f, 0.f};
    Quat Rotation{0.f, 0.f, 0.f, 1.f};


    void MarkFramesDirty()
    {
        NumFramesDirty = kNumFramesMarkDirty;
    };


public:
    TransformComponent() = default;
    TransformComponent(const TransformComponent &) = default;

    TransformComponent(const Vec3 &translation)
        : Translation(translation)
    {
    }

    bool IsDirty() const
    {
        return (NumFramesDirty - (kNumFramesMarkDirty - 1)) > 0;
    }

    bool IsFramesDirty() const
    {
        return NumFramesDirty > 0;
    }

    void FrameResetDirtyFlags()
    {
        NumFramesDirty > 0 ? --NumFramesDirty : NumFramesDirty;
    }

    Mat4 GetTransform() const
    {
        BLAINN_PROFILE_FUNC();
        using namespace DirectX::SimpleMath;
        return Matrix::CreateScale(Scale) * Matrix::CreateFromQuaternion(Rotation)
               * Matrix::CreateTranslation(Translation);
    }

    void SetTransform(Mat4 &transform)
    {
        BLAINN_PROFILE_FUNC();
        using namespace DirectX::SimpleMath;
        transform.Decompose(Scale, Rotation, Translation);
        EulerRotation = Rotation.ToEuler();

        MarkFramesDirty();
    }

    Vec3 GetTranslation() const
    {
        return Translation;
    }

    void SetTranslation(const Vec3 &translation)
    {
        Translation = translation;
        MarkFramesDirty();
    }

    Vec3 GetScale() const
    {
        return Scale;
    }

    void SetScale(const Vec3 &scale)
    {
        Scale = scale;
        MarkFramesDirty();
    }

    Vec3 GetRotationEuler() const
    {
        return EulerRotation;
    }

    void SetRotationEuler(const Vec3 &euler)
    {
        BLAINN_PROFILE_FUNC();
        using namespace DirectX::SimpleMath;
        EulerRotation = euler;
        Rotation = Quaternion::CreateFromYawPitchRoll(euler.x, euler.y, euler.z);
        MarkFramesDirty();
    }

    Quat GetRotation() const
    {
        return Rotation;
    }

    void SetRotation(const Quat &rotation)
    {
        BLAINN_PROFILE_FUNC();
        using namespace DirectX::SimpleMath;
        auto warpToPi = [](const Vec3 v)
        {
            constexpr auto piVec = Vec3(DirectX::XM_PI);
            const auto x = v + piVec;
            const auto y = 2 * piVec;

            const auto mod = x - y * Vec3(DirectX::XMVectorFloor(x / y));
            return mod - piVec;
        };

        const auto originalEuler = EulerRotation;
        Rotation = rotation;
        EulerRotation = Rotation.ToEuler();

        Vec3 alternatives[4] = {
            {EulerRotation.y - DirectX::XM_PI, EulerRotation.x - DirectX::XM_PI, EulerRotation.z - DirectX::XM_PI},
            {EulerRotation.y - DirectX::XM_PI, EulerRotation.x + DirectX::XM_PI, EulerRotation.z - DirectX::XM_PI},
            {EulerRotation.y - DirectX::XM_PI, EulerRotation.x + DirectX::XM_PI, EulerRotation.z + DirectX::XM_PI},
            {EulerRotation.y - DirectX::XM_PI, EulerRotation.x - DirectX::XM_PI, EulerRotation.z + DirectX::XM_PI}};

        float distances[5] = {(warpToPi(EulerRotation - originalEuler)).LengthSquared(),
                              (warpToPi(alternatives[0] - originalEuler)).LengthSquared(),
                              (warpToPi(alternatives[1] - originalEuler)).LengthSquared(),
                              (warpToPi(alternatives[2] - originalEuler)).LengthSquared(),
                              (warpToPi(alternatives[3] - originalEuler)).LengthSquared()};

        float best = distances[0];
        int bestIndex = 0;
        for (const auto &distance : distances)
        {
            if (distance < best)
            {
                best = distance;
                EulerRotation = alternatives[bestIndex];
            }
            bestIndex++;
        }

        EulerRotation = warpToPi(EulerRotation);
        MarkFramesDirty();
    }


    Vec3 GetForwardVector() const
    {
        using namespace DirectX;
        Vec3 forward{Vec3::UnitZ};
        forward = Vec3::Transform(forward, Rotation);
        return forward;
    }

    Vec3 GetRightVector() const
    {
        using namespace DirectX;
        Vec3 right{Vec3::UnitX};
        right = Vec3::Transform(right, Rotation);
        return right;
    }

    Vec3 GetUpVector() const
    {
        using namespace DirectX;
        Vec3 up{Vec3::UnitY};
        up = Vec3::Transform(up, Rotation);
        return up;
    }
};
} // namespace Blainn