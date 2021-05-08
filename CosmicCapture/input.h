#pragma once

#include <map>
#include <vector>

#include "SDL/SDL.h"

enum class MovementFlags
{
    LEFT, RIGHT, UP, DOWN, ACTION, ENTER, RESET, OTHER_ACTION
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
        inputState[MovementFlags::OTHER_ACTION] = true;

        prevInputState = inputState;
    }

    // Inputs that are not currently held
    std::map<MovementFlags, bool> inputState;

    // Inputs that where not held last frame
    std::map<MovementFlags, bool> prevInputState;

	bool inputReleased(const MovementFlags& movementFlag)
	{
        if (!prevInputState[movementFlag] && inputState[movementFlag])
        {
            prevInputState[movementFlag] = true;
            return true;
        }

        return false;
	}
};


class Input
{
public:
    /// <summary>
    /// Process input
    /// </summary>
    /// <returns>True if the user wants to quit. False otherwise</returns>
    bool HandleInput();

    [[nodiscard]] InputInfo* getInfo();
    [[nodiscard]] InputInfo* getInfo(int);
    [[nodiscard]] std::map<int, InputInfo>* getAllControllerInfo() { return &mControllerInfo; }

	// Mouse stuff
    bool mouseHeld = false;
    int mouseX, mouseY;

private:
	SDL_Event mEvent;

    // Information about input events that come from controllers
    std::map<int, InputInfo> mControllerInfo;

    // Information about input events that come from keyboards
    InputInfo mKeyboardInfo;

    std::vector<SDL_GameController*> mControllers;

    //Analog joystick dead zone
    const int JOYSTICK_DEAD_ZONE = 8000;

    void HandleKeys();
    void HandleButtons();
    void HandleJoystick();
    void HandleMouse();
    void HandleMouseMove();
};