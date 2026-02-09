#pragma once

#include <random>

#include "aliases.h"

namespace Blainn
{

class Rand
{
public:
    inline static const uuid getRandomUUID()
    {
        static thread_local Blainn::UUIDGenerator<std::mt19937_64> generator;
        return generator.getUUID();
    }

    inline static const std::shared_ptr<std::mt19937_64> getRNG()
    {
        return m_rng;
    }

private:
    inline static std::shared_ptr<std::mt19937_64> m_rng = std::make_shared<std::mt19937_64>(std::random_device{}());
};

} // namespace Blainn