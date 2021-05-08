#include "Timer.h"

#include <SDL/SDL_timer.h>

void Timer::start()
{
	mStarted = true;
	mPaused = false;

	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void Timer::stop()
{
	mStarted = false;
	mPaused = false;

	mStartTicks = 0;
	mPausedTicks = 0;
}

void Timer::pause()
{
	if (mStarted && !mPaused)
	{
		mPaused = true;

		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void Timer::unpause()
{
	if (mStarted && mPaused)
	{
		mPaused = false;

		mStartTicks = SDL_GetTicks() - mPausedTicks;
		mPausedTicks = 0;
	}
}

std::uint32_t Timer::getTicks()
{
	std::uint32_t time = 0;

	if (mStarted)
	{
		if (mPaused)
			time = mPausedTicks;
		else
			time = SDL_GetTicks() - mStartTicks;
	}

	return time;
}
