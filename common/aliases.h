#pragma once

#include <uuid_v4.h>
#include "DirectXTK12/inc/SimpleMath.h"
#include <filesystem>

namespace Blainn
{
using uuid = UUIDv4::UUID;

using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;

using Quat = DirectX::SimpleMath::Quaternion;
using Mat4 = DirectX::SimpleMath::Matrix;

using Plane = DirectX::SimpleMath::Plane;
using Rect = DirectX::SimpleMath::Rectangle;
using Ray = DirectX::SimpleMath::Ray;

using Viewport = DirectX::SimpleMath::Viewport;
using Color = DirectX::SimpleMath::Color;

using Path = std::filesystem::path;
} // namespace Blainn