#include <SDL/SDL.h>
#include <SDL/SDL_gamecontroller.h>
#include <iostream>
#include <fstream>

/*
  Code sources used in this file:
  https://lazyfoo.net/tutorials/SDL/19_gamepads_and_joysticks/index.php
  https://fossies.org/diffs/SDL2/2.0.8_vs_2.0.9/test/testgamecontroller.c-diff.html
  https://www.falukdevelop.com/2016/11/02/sdl2-controller-implementation/

*/

class Input
{
public:
    //--- Constructor
    Input();

    // Handling all input in one method at the moment, will split up later
    void HandleEvent(SDL_Event& event);
    void HandleKeys(SDL_Event& event, int pressed);
    void HandleButtons(SDL_Event& event, int pressed);
    void HandleJoystick(SDL_Event& event);

    
    //Analog joystick dead zone
    const int JOYSTICK_DEAD_ZONE = 8000;

private:
    // turning movement
    int xDir;
    // forwards/back movement
    int yDir;
    // driving i.e. foot on the gas
    bool driving;
};



Input:: Input()
{
    xDir = 0;
    yDir = 0;
    driving = false;
}

void Input::HandleEvent(SDL_Event& event) {

    switch (event.type)
    {
    case SDL_KEYDOWN:
        HandleKeys(event, 1);
        break;
    case SDL_KEYUP:
        HandleKeys(event, 0);
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        HandleButtons(event, 1);
    case SDL_CONTROLLERBUTTONUP:
        HandleButtons(event, 0);
        break;
    case SDL_CONTROLLERAXISMOTION:
        HandleJoystick(event);
        break;
    case SDL_CONTROLLERDEVICEADDED:
        std::cout << "Controller added \n";
        break;
    case SDL_CONTROLLERDEVICEREMOVED:
        std::cout << "Controller removed \n";
        break;
    default:
        return;

    }
}

void Input::HandleKeys(SDL_Event& event, int pressed) {

    if (pressed == 1)
        std::cout << "Key pressed: ";
    else
        std::cout << "Key released: ";

    switch (event.key.keysym.sym) {

    case SDLK_a:
        std::cout << "left \n";
        xDir = pressed * -1;
        break;
    case SDLK_d:
        std::cout << "right \n";
        xDir = pressed * 1;
        break;
    case SDLK_w:
        std::cout << "up \n";
        yDir = pressed * 1;
        break;
    case SDLK_s:
        std::cout << "down \n";
        yDir = pressed * -1;
        break;
    case SDLK_SPACE:
        std::cout << "go (space) \n";
        driving = (pressed == 1);
        break;
    default:
        std::cout << "irrelevant \n";
        return;

    }

}

void Input::HandleButtons(SDL_Event& event, int pressed) {

    if (pressed == 1)
        std::cout << "C. Button pressed: ";
    else
        std::cout << "C. Button released: ";

    // Will add cases as more buttons become necessary
    switch (event.cbutton.button) {
        case SDL_CONTROLLER_BUTTON_A:
            std::cout << "go (A) \n";
            driving = (pressed == 1);
            break;
        case SDL_CONTROLLER_BUTTON_B:
            std::cout << "pressed (B) \n";
            break;
        case SDL_CONTROLLER_BUTTON_X:
            std::cout << "pressed (X) \n";
            break;
        case SDL_CONTROLLER_BUTTON_Y:
            std::cout << "pressed (Y) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            std::cout << "pressed (DPAD_DOWN) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            std::cout << "pressed (DPAD_UP) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            std::cout << "pressed (DPAD_LEFT) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            std::cout << "pressed (DPAD_RIGHT) \n";
            break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
            std::cout << "pressed (RIGHTSHOULDER) \n";
            break;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
            std::cout << "pressed (LEFTSHOULDER) \n";
            break;
        case SDL_CONTROLLER_BUTTON_START:
            std::cout << "pressed (START) \n";
            break;
        default:
            std::cout << "irrelevant \n";
            return;

    }

}

void Input::HandleJoystick(SDL_Event& event) {

    if(event.caxis.value < -JOYSTICK_DEAD_ZONE || event.caxis.value > JOYSTICK_DEAD_ZONE)
        std::cout << "Axis: ";

    //X axis motion

    if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
    {
        //Left of dead zone
        if (event.caxis.value < -JOYSTICK_DEAD_ZONE)
        {
            std::cout << "left \n";
            xDir = -1;
        }
        //Right of dead zone
        else if (event.caxis.value > JOYSTICK_DEAD_ZONE)
        {
            std::cout << "right \n";
            xDir = 1;
        }
        else
        {
            xDir = 0;
        }

    } //Y axis motion

    else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
    {
        //Below dead zone
        if (event.caxis.value < -JOYSTICK_DEAD_ZONE)
        {
            std::cout << "down \n";
            yDir = -1;
        }
        //Above dead zone
        else if (event.caxis.value > JOYSTICK_DEAD_ZONE)
        {
            std::cout << "up \n";
            yDir = 1;
        }
        else
        {
            yDir = 0;
        }

    }
    else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
        std::cout << "pressed (TRIGGERLEFT) \n";
    }
    else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
        std::cout << "pressed (TRIGGERRIGHT) \n";
    }
    else {
    
        std::cout << "Use left joystick \n";
    }

}

