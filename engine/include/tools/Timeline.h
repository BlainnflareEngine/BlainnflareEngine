#pragma once 

namespace Blainn {
	// TODO:
	class Timeline {
	// TODO: something like:
	public:
		Timeline();
		~Timeline();
		void Start();
		void Stop();
		void Reset();
		void Update();
		float GetDeltaTime() const;
		float GetElapsedTime() const;
	private:
		bool running;
		float startTime;
		float elapsedTime;
		float deltaTime;
		float lastUpdateTime;
		float GetCurrentTime() const; 
	};
} // namespace Blainn