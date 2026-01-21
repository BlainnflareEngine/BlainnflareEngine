//
// Created by WhoLeb on 25-Dec-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//


#pragma once

#include "Camera.h"

namespace Blainn {
    class RuntimeCamera : public Camera
    {
        using Super = Camera;
    public:
        virtual ~RuntimeCamera() = default;

        void Update(float dt) override;
        void SetViewMatrix(const Mat4& viewMat) { m_view = viewMat; }

    private:
    };
} // Blainn