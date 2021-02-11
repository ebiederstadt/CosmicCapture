#ifndef __INPUT_H__
#define __INPUT_H__
#pragma once
///////////////////////////////
#include "Physics.h"
#include "SDL/SDL.h"
class Input
{
public:
    //--- Constructor
    Input(Physics physics);
    // Handling all input in one method at the moment, will split up later
    void HandleKeys(SDL_Event& event);
    void HandleButtons(SDL_Event& event);
    void HandleJoystick(SDL_Event& event);
    void HandleMouse(SDL_Event& event, int pressed);

    bool leftup;
    bool rightup;
    bool upup;
    bool downup;
    bool reverseup;

    //Analog joystick dead zone
    const int JOYSTICK_DEAD_ZONE = 8000;

    //Game controller 1 handler
    SDL_Joystick* gGameController = NULL;

    bool getUpUp();
    bool getDownUp();
    bool getLeftUp();
    bool getRightUp();

private:
    Physics physics;
    // turning movement
    //int xDir;
    // forwards/back movement
    //int yDir;
    // driving i.e. foot on the gas
    //bool driving;
};
///////////////////////////////
#endif   //__INPUT_H__