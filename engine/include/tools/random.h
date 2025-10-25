#pragma once

#include <random>
#include <uuid_v4.h>

#include "aliases.h"

namespace Blainn
{

class Rand
{
public:
    inline static const uuid getRandomUUID()
    {
        static thread_local UUIDv4::UUIDGenerator<std::mt19937_64> generator;
        return generator.getUUID();
    }

    inline static const std::shared_ptr<std::mt19937_64> getRNG()
    {
        return m_rng;
    }

    // TODO: genneration functions if needed
private:
    // TODO: thread local?
    inline static std::shared_ptr<std::mt19937_64> m_rng = std::make_shared<std::mt19937_64>(std::random_device{}());
};

} // namespace Blainn