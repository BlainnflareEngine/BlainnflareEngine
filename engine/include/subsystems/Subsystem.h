#pragma once

namespace Blainn
{
// Base class for all subsystems
class Subsystem
{
public:
    virtual ~Subsystem() = default;
    virtual void Init() = 0;
    virtual void Destroy() = 0;
    // TODO:?
};
} // namespace Blainn