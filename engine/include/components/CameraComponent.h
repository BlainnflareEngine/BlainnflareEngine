//
// Created by WhoLeb on 25-Dec-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#pragma once

#include "Render/RuntimeCamera.h"

namespace Blainn {
    struct CameraComponent {
        RuntimeCamera camera;
        int32_t CameraPriority = 0;
    };
}
