#pragma once 

#include <cstdint>

#include <EASTL/chrono.h>

namespace Blainn {

	namespace {
		template<typename T>
		struct is_eastl_duration : eastl::false_type {};

		template<typename Rep, typename Period>
		struct is_eastl_duration<eastl::chrono::duration<Rep, Period>> : eastl::true_type {};
	}

	template<typename EastlDurationType>
	class Timeline {
		static_assert(is_eastl_duration<EastlDurationType>::value,
            "EastlDurationType must be a eastl::chrono::duration specialization");
	public:
		Timeline(int64_t period);
		void Start();
		void Pause();
		void Resume();
		// returns amount of cycles done since last update
		int64_t Tick();
		void Reset();
		// since = -1 means since last update
		int64_t GetElapsedTimeSince(int64_t since = -1) const;
	private:
		bool m_isActive;
		int64_t m_period;
		int64_t m_startTime;
		int64_t m_prevUpdateTime;
		int64_t m_lastSavedTime;
		int64_t GetTime() const; 
	};

	template<typename EastlDurationType>
	Blainn::Timeline<EastlDurationType>::Timeline(int64_t period)
	{
		m_period = period;
		Reset();
	}

	template<typename EastlDurationType>
	void Blainn::Timeline<EastlDurationType>::Start()
	{
		if (m_isActive) return;

		m_isActive = true;
		m_startTime = GetTime();
		m_lastSavedTime = m_startTime;
		m_prevUpdateTime = m_startTime;
	}

	template<typename EastlDurationType>
	void Blainn::Timeline<EastlDurationType>::Pause()
	{
		if (m_isActive) m_isActive = false;
	}

	template<typename EastlDurationType>
	void Blainn::Timeline<EastlDurationType>::Resume()
	{
		if (!m_isActive) m_isActive = true;
	}

	template<typename EastlDurationType>
	int64_t Blainn::Timeline<EastlDurationType>::Tick()
	{
		if (!m_isActive) return 0;

		m_lastSavedTime = GetTime();
		int64_t elapsedTime = m_lastSavedTime - m_prevUpdateTime;
		int64_t cyclesDone = elapsedTime / m_period;
		if (cyclesDone > 0) m_prevUpdateTime += cyclesDone * m_period;
		return cyclesDone;
	}

	template<typename EastlDurationType>
	void Blainn::Timeline<EastlDurationType>::Reset()
	{
		m_isActive = false;
		m_startTime = 0;
		m_prevUpdateTime = 0;
		m_lastSavedTime = 0;
	}

	template<typename EastlDurationType>
	int64_t Blainn::Timeline<EastlDurationType>::GetElapsedTimeSince(int64_t since /* = -1*/) const
	{
		return since == -1 ? m_lastSavedTime - m_prevUpdateTime : m_lastSavedTime - since;
	}

	template<typename EastlDurationType>
	int64_t Blainn::Timeline<EastlDurationType>::GetTime() const
	{
		return eastl::chrono::duration_cast<EastlDurationType>(eastl::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}

} // namespace Blainn