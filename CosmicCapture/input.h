#ifndef __INPUT_H__
#define __INPUT_H__
#pragma once
///////////////////////////////

#include <map>
#include <vector>

#include "SDL/SDL.h"

enum class MovementFlags
{
    LEFT, RIGHT, UP, DOWN, ACTION, ENTER, RESET
};

// Information about the input
struct InputInfo
{
    InputInfo()
    {
		inputState[MovementFlags::LEFT] = true;
		inputState[MovementFlags::RIGHT] = true;
		inputState[MovementFlags::DOWN] = true;
		inputState[MovementFlags::UP] = true;
		inputState[MovementFlags::ACTION] = true;
		inputState[MovementFlags::ENTER] = true;
		inputState[MovementFlags::RESET] = true;

        prevInputState = inputState;
    }
	
    bool keyboard = false;
    bool controller = false;
    int controllerID = 0;

    // Inputs that are not currently held
    std::map<MovementFlags, bool> inputState;

    // Inputs that where not held last frame
    std::map<MovementFlags, bool> prevInputState;

	void setKeyboard()
	{
        keyboard = true;
        controller = false;
	}

	void setController(int id)
	{
        keyboard = false;
        controller = true;
        controllerID = id;
	}
};


class Input
{
public:
    /// <summary>
    /// Process input
    /// </summary>
    /// <param name="event">The input event to process</param>
    /// <returns>True if the user wants to quit. False otherwise</returns>
    bool HandleInput();

    [[nodiscard]] InputInfo getInfo() const { return mInfo; }

	// Mouse stuff
    bool mouseHeld = false;
    int mouseX, mouseY;

    // Returns true when the input was held on the previous frame and is now released
    bool inputReleased(const MovementFlags& movementFlag) {
        if (!mInfo.prevInputState[movementFlag] && mInfo.inputState[movementFlag])
        {
            mInfo.prevInputState[movementFlag] = true;
            return true;
        }

        return false;
    }


private:
	SDL_Event mEvent;

    // Information about the most recent input event
    InputInfo mInfo;

    std::vector<SDL_GameController*> mControllers;

    //Analog joystick dead zone
    const int JOYSTICK_DEAD_ZONE = 8000;

    void HandleKeys();
    void HandleButtons();
    void HandleJoystick();
    void HandleMouse();
    void HandleMouseMove();
};
///////////////////////////////
#endif   //__INPUT_H__