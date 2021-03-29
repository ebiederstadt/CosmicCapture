#ifndef __INPUT_H__
#define __INPUT_H__
#pragma once
///////////////////////////////

#include <map>

#include "SDL/SDL.h"

enum class MovementFlags
{
    LEFT, RIGHT, UP, DOWN, ACTION, ENTER, RESET
};


class Input
{
public:
    //--- Constructor
    Input();

    /// <summary>
    /// Process input
    /// </summary>
    /// <param name="event">The input event to process</param>
    /// <returns>True if the user wants to quit. False otherwise</returns>
    bool HandleInput();

    [[nodiscard]] std::map<MovementFlags, bool> getInputState() const { return mInputMap; }

	// Mouse stuff
    bool mouseHeld = false;
    int mouseX, mouseY;


private:
	SDL_Event mEvent;

    // Inputs that are currently not held
    std::map<MovementFlags, bool> mInputMap;

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