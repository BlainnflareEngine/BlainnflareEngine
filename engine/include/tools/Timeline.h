#pragma once

namespace Blainn
{

namespace
{
template <typename T> struct is_eastl_duration : eastl::false_type
{
};

template <typename Rep, typename Period>
struct is_eastl_duration<eastl::chrono::duration<Rep, Period>> : eastl::true_type
{
};
} // namespace

template <typename EastlDurationType> class Timeline
{
    static_assert(is_eastl_duration<EastlDurationType>::value,
                  "EastlDurationType must be a eastl::chrono::duration specialization");

public:
    Timeline(Timeline *parentTimeline);
    virtual ~Timeline();
    void Start();
    void Pause();
    void Resume();
    bool isActive() const;
    // returns amount of time since last update
    virtual float Tick();
    void Reset();

    void AddChildTimeline(Timeline *childTimeline);
    void RemoveChildTimeline(Timeline *childTimeline);

private:
    float GetLastSavedTickDelta() const;

    bool m_isActive;
    int64_t m_startTime;
    int64_t m_lastSavedTime;
    int64_t m_lastSavedTickDelta;
    int64_t GetTime() const;

    Timeline *m_parentTimeline = nullptr;
    eastl::vector<Timeline *> m_childTimelines;
};

template <typename EastlDurationType> Blainn::Timeline<EastlDurationType>::Timeline(Timeline *parentTimeline)
{
    Reset();
    m_parentTimeline = parentTimeline;
    if (parentTimeline) parentTimeline->AddChildTimeline(this);
}

template <typename EastlDurationType> inline Timeline<EastlDurationType>::~Timeline()
{
    if (m_parentTimeline)
    {
        m_parentTimeline->RemoveChildTimeline(this);
    }
}

template <typename EastlDurationType> void Blainn::Timeline<EastlDurationType>::Start()
{
    if (m_isActive) return;

    m_isActive = true;
    m_startTime = GetTime();
    m_lastSavedTime = m_startTime;

    for (Timeline *childTimeline : m_childTimelines)
    {
        childTimeline->Start();
    }
}

template <typename EastlDurationType> void Blainn::Timeline<EastlDurationType>::Pause()
{
    if (!m_isActive) return;
    m_isActive = false;

    for (Timeline *childTimeline : m_childTimelines)
    {
        childTimeline->Pause();
    }
}

template <typename EastlDurationType> void Blainn::Timeline<EastlDurationType>::Resume()
{
    if (m_isActive) return;
    m_isActive = true;

    for (Timeline *childTimeline : m_childTimelines)
    {
        childTimeline->Resume();
    }
}

template <typename EastlDurationType> float Blainn::Timeline<EastlDurationType>::Tick()
{
    if (!m_isActive) return 0.0f;
    if (m_parentTimeline) return m_parentTimeline->GetLastSavedTickDelta();

    int64_t prevTime = m_lastSavedTime;
    m_lastSavedTime = GetTime();
    m_lastSavedTickDelta = m_lastSavedTime - prevTime;
    return static_cast<float>(m_lastSavedTickDelta);
}

template <typename EastlDurationType> void Blainn::Timeline<EastlDurationType>::Reset()
{
    m_isActive = false;
    m_startTime = 0;
    m_lastSavedTime = 0;

    for (Timeline *childTimeline : m_childTimelines)
    {
        childTimeline->Reset();
    }
}

template <typename EastlDurationType> inline void Timeline<EastlDurationType>::AddChildTimeline(Timeline *childTimeline)
{
    m_childTimelines.push_back(childTimeline);
}

template <typename EastlDurationType>
inline void Timeline<EastlDurationType>::RemoveChildTimeline(Timeline *childTimeline)
{
    if (m_childTimelines.size() == 0 ) return;

    auto it = eastl::find(m_childTimelines.begin(), m_childTimelines.end(), childTimeline);
    if (it != m_childTimelines.end())
    {
        m_childTimelines.erase(it);
    }
}

template <typename EastlDurationType> inline float Timeline<EastlDurationType>::GetLastSavedTickDelta() const
{
    return m_lastSavedTickDelta;
}

template <typename EastlDurationType> int64_t Blainn::Timeline<EastlDurationType>::GetTime() const
{
    return eastl::chrono::duration_cast<EastlDurationType>(
               eastl::chrono::high_resolution_clock::now().time_since_epoch())
        .count();
}

template <typename EastlDurationType> bool Blainn::Timeline<EastlDurationType>::isActive() const
{
    if (m_parentTimeline)
    {
        return m_isActive && m_parentTimeline->isActive();
    }
    return m_isActive;
}
} // namespace Blainn