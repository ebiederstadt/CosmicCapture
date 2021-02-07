#ifndef __INPUT_H__
#define __INPUT_H__
#pragma once
///////////////////////////////
#include "Physics.h"

class Input
{
public:
    //--- Constructor
    Input(Physics physics);

    // Handling all input in one method at the moment, will split up later
    bool HandleEvent(SDL_Event& event);
    void HandleKeys(SDL_Event& event, int pressed);
    void HandleButtons(SDL_Event& event, int pressed);
    void HandleJoystick(SDL_Event& event);
    void HandleMouse(SDL_Event& event, int pressed);


    //Analog joystick dead zone
    const int JOYSTICK_DEAD_ZONE = 8000;

private:
    Physics physics;
    // turning movement
    int xDir;
    // forwards/back movement
    int yDir;
    // driving i.e. foot on the gas
    bool driving;
};
///////////////////////////////
#endif   //__INPUT_H__