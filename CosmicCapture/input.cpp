#include <SDL/SDL.h>
#include <SDL/SDL_gamecontroller.h>
#include <iostream>
#include <fstream>
#include <fmt/format.h>
#include "input.h"


/*
  Code sources used in this file:
  https://lazyfoo.net/tutorials/SDL/19_gamepads_and_joysticks/index.php
  https://fossies.org/diffs/SDL2/2.0.8_vs_2.0.9/test/testgamecontroller.c-diff.html
  https://www.falukdevelop.com/2016/11/02/sdl2-controller-implementation/

*/

Input::Input(Physics phys)
{
    physics = phys;
    leftup = true;
    rightup = true;
    downup = true;
    upup = true;
    reverseup = true;
    //driving = false;
}

bool Input::getUpUp() {
    return upup;
}

bool Input::getDownUp() {
    return downup;
}

bool Input::getLeftUp() {
    return leftup;
}

bool Input::getRightUp() {
    return rightup;
}


void Input::HandleKeys(SDL_Event& event) {

    if (event.type == SDL_KEYDOWN) {
        //std::cout << "Key pressed: ";
        switch (event.key.keysym.sym) {
        case SDLK_a:
            //std::cout << "left \n";
            leftup = false;
            break;
        case SDLK_d:
            //std::cout << "right \n";
            rightup = false;
            break;
        case SDLK_w:
            //std::cout << "up \n";
            upup = false;
            break;
        case SDLK_s:
            //std::cout << "down \n";
            downup = false;
            break;
        default:
            //std::cout << "irrelevant \n";
            break;
        }
    }
    if (event.type == SDL_KEYUP) {
        //std::cout << "Key released: ";
        switch (event.key.keysym.sym) {
        case SDLK_a:
            //std::cout << "left \n";
            leftup = true;
            break;
        case SDLK_d:
            //std::cout << "right \n";
            rightup = true;
            break;
        case SDLK_w:
            //std::cout << "up \n";
            upup = true;
            break;
        case SDLK_s:
            //std::cout << "down \n";
            downup = true;
            break;
        default:
            //std::cout << "irrelevant \n";
            break;
        }
    }


}

void Input::HandleButtons(SDL_Event& event) {

    if (event.type == SDL_JOYBUTTONDOWN) {
        //std::cout << "C. Button pressed: ";
        // Will add cases as more buttons become necessary
        switch (event.cbutton.button) {
        case SDL_CONTROLLER_BUTTON_A: //a to go forward
            //std::cout << "go (A) \n";
            upup = false;
            break;
        case SDL_CONTROLLER_BUTTON_B://b to brake
            //std::cout << "pressed (B) \n";
            downup = false;
            break;
        case SDL_CONTROLLER_BUTTON_X:
            //std::cout << "pressed (X) \n";
            break;
        case SDL_CONTROLLER_BUTTON_Y:
            //std::cout << "pressed (Y) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            //std::cout << "pressed (DPAD_DOWN) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            ///std::cout << "pressed (DPAD_UP) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            //std::cout << "pressed (DPAD_LEFT) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            //std::cout << "pressed (DPAD_RIGHT) \n";
            break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
            //std::cout << "pressed (RIGHTSHOULDER) \n";
            break;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
            //std::cout << "pressed (LEFTSHOULDER) \n";
            break;
        case SDL_CONTROLLER_BUTTON_START:
            //std::cout << "pressed (START) \n";
            break;
        default:
            //std::cout << "irrelevant \n";
            return;

        }
    }
    if (event.type == SDL_JOYBUTTONUP) {
        //std::cout << "C. Button Released: ";
        // Will add cases as more buttons become necessary
        switch (event.cbutton.button) {
        case SDL_CONTROLLER_BUTTON_A: //a to go forward
            //std::cout << "released (A) \n";
            upup = true;
            break;
        case SDL_CONTROLLER_BUTTON_B://b to brake
            //std::cout << "released (B) \n";
            downup = true;
            break;
        case SDL_CONTROLLER_BUTTON_X:
            //std::cout << "released (X) \n";
            break;
        case SDL_CONTROLLER_BUTTON_Y:
            //std::cout << "released (Y) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            //std::cout << "released (DPAD_DOWN) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            //std::cout << "released (DPAD_UP) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            //std::cout << "released (DPAD_LEFT) \n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            //std::cout << "released (DPAD_RIGHT) \n";
            break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
            //std::cout << "released (RIGHTSHOULDER) \n";
            break;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
            //std::cout << "released (LEFTSHOULDER) \n";
            break;
        case SDL_CONTROLLER_BUTTON_START:
            //std::cout << "released (START) \n";
            break;
        default:
            //std::cout << "irrelevant \n";
            return;

        }
    }

}

void Input::HandleJoystick(SDL_Event& event) {

    //if(event.caxis.value < -JOYSTICK_DEAD_ZONE || event.caxis.value > JOYSTICK_DEAD_ZONE)
        //std::cout << "Axis: ";

    //X axis motion

    if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
    {
        //Left of dead zone
        if (event.caxis.value < -JOYSTICK_DEAD_ZONE)
        {
            //std::cout << "left \n";
            leftup = false;
            rightup = true;
        }
        //Right of dead zone
        if (event.caxis.value > JOYSTICK_DEAD_ZONE)
        {
            //std::cout << "right \n";
            rightup = false;
            leftup = true;
        }
        //no joystick movement at all, dead center
        if (event.caxis.value <= JOYSTICK_DEAD_ZONE && event.caxis.value >= -JOYSTICK_DEAD_ZONE) {
            leftup = true;
            rightup = true;
        }

    } //Y axis motion

    else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
    {
        //Below dead zone
        if (event.caxis.value < -JOYSTICK_DEAD_ZONE)
        {
            //std::cout << "down \n";
            //yDir = -1;
        }
        //Above dead zone
        else if (event.caxis.value > JOYSTICK_DEAD_ZONE)
        {
            //std::cout << "up \n";
            //yDir = 1;
        }
        else
        {
           //yDir = 0;
        }

    }
    else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
        //std::cout << "pressed (TRIGGERLEFT) \n";
    }
    else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
        //std::cout << "pressed (TRIGGERRIGHT) \n";
    }
    else {
    
        //std::cout << "Use left joystick \n";
    }

}

void Input::HandleMouse(SDL_Event& event, int pressed)
{
    if (pressed == 1)
        fmt::print("Pressed: ");
    else
        fmt::print("Released: ");

    if (event.button.type == SDL_BUTTON_LEFT)
        fmt::print("Left mouse button\n");
    else if (event.button.type == SDL_BUTTON_MIDDLE)
        fmt::print("Middle mouse button\n");
    else if (event.button.type == SDL_BUTTON_RIGHT)
        fmt::print("Right mouse button");
}

