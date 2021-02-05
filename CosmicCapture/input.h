#ifndef __INPUT_H__
#define __INPUT_H__
#pragma once
///////////////////////////////

class Input
{
public:
    ///--- Constructor
    Input();

    // handle controller input
    // Handling all input in one method at the moment, will split up later
    void HandleEvent(SDL_Event& event);
    void HandleKeys(SDL_Event& event, int pressed);
    void HandleButtons(SDL_Event& event, int pressed);
    void HandleJoystick(SDL_Event& event);


private:
    int xDir;
    int yDir;
    bool driving;
};

///////////////////////////////
#endif   //__INPUT_H__