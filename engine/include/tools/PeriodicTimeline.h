#pragma once

#include "Timeline.h"

namespace Blainn
{

// A timeline that triggers at fixed intervals (periods).
template <typename EastlDurationType> class PeriodicTimeline final : public Timeline<EastlDurationType>
{
    static_assert(is_eastl_duration<EastlDurationType>::value,
                  "EastlDurationType must be a eastl::chrono::duration specialization");

public:
    PeriodicTimeline(float period, Timeline<EastlDurationType> *parentTimeline)
        : m_period(period)
        , Timeline<EastlDurationType>(parentTimeline) {};

    // returns period time if it has elapsed otherwise 0.0f
    float Tick() override;

private:
    float m_period;
    float m_accumulatedTime = 0.0f;
};

template <typename EastlDurationType> inline float PeriodicTimeline<EastlDurationType>::Tick()
{
    m_accumulatedTime += Timeline<EastlDurationType>::Tick();
    if (m_accumulatedTime >= m_period)
    {
        m_accumulatedTime -= m_period;
        return m_period;
    }
    return 0.0f;
}

} // namespace Blainn