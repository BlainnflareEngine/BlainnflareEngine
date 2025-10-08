//
// Created by gorev on 08.10.2025.
//

#pragma once
#include "aliases.h"

namespace Blainn
{
static UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;

inline uuid GenerateID()
{
    return uuidGenerator.getUUID();
}
} // namespace Blainn