#pragma once

#include <cstdint>

class Timer
{
public:
	void start();
	void stop();
	void pause();
	void unpause();

	std::uint32_t getTicks();

	[[nodiscard]] bool isStarted() const { return mStarted; }
	[[nodiscard]] bool isPaused() const { return  mPaused; }

private:
	std::uint32_t mStartTicks = 0;
	std::uint32_t mPausedTicks = 0;

	bool mPaused = false;
	bool mStarted = false;
};

