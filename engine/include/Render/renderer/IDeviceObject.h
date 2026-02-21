//
// Created by WhoLeb on 21-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include "Render/GraphicsTypes.h"

namespace Blainn
{
struct IDeviceObject
{
    virtual const DeviceObjectAttribs& GetDesc() const = 0;

    virtual uuid GetUUID() const = 0;

    virtual void SetUserData(const void* _pUserData) = 0;

    virtual void* GetUserData() const = 0;
};
}