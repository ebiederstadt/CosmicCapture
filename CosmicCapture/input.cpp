#include <SDL/SDL.h>
#include <SDL/SDL_gamecontroller.h>
#include "input.h"



/*
  Code sources used in this file:
  https://lazyfoo.net/tutorials/SDL/19_gamepads_and_joysticks/index.php
  https://fossies.org/diffs/SDL2/2.0.8_vs_2.0.9/test/testgamecontroller.c-diff.html
  https://www.falukdevelop.com/2016/11/02/sdl2-controller-implementation/

*/

Input::Input()
{
    mInputMap[MovementFlags::LEFT] = true;
    mInputMap[MovementFlags::RIGHT] = true;
    mInputMap[MovementFlags::DOWN] = true;
    mInputMap[MovementFlags::UP] = true;
    mInputMap[MovementFlags::ACTION] = true;
    mInputMap[MovementFlags::ENTER] = true;
}

bool Input::HandleInput()
{
    bool quit = false;
	while (SDL_PollEvent(&mEvent) != 0) {
		switch (mEvent.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			HandleKeys();
			break;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			HandleButtons();
			break;
		case SDL_JOYAXISMOTION:
            HandleJoystick();
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			HandleMouse();
			break;
		case SDL_MOUSEMOTION:
			HandleMouseMove();
			break;
		}
	}
    return quit;
}

void Input::HandleKeys()
{
    const bool keyReleased = mEvent.type == SDL_KEYUP;
	switch (mEvent.key.keysym.sym) {
	case SDLK_a:
        mInputMap[MovementFlags::LEFT] = keyReleased;
		break;
	case SDLK_d:
        mInputMap[MovementFlags::RIGHT] = keyReleased;
		break;
	case SDLK_w:
        mInputMap[MovementFlags::UP] = keyReleased;
		break;
	case SDLK_s:
        mInputMap[MovementFlags::DOWN] = keyReleased;
		break;
    case SDLK_SPACE:
        mInputMap[MovementFlags::ACTION] = keyReleased;
        break;
    case SDLK_RETURN:
        mInputMap[MovementFlags::ENTER] = keyReleased;
        break;
	default:
		break;
	}
}

void Input::HandleButtons()
{
    const bool buttonReleased = mEvent.type == SDL_JOYBUTTONUP;
	// Will add cases as more buttons become necessary
	switch (mEvent.cbutton.button) {
	case SDL_CONTROLLER_BUTTON_A: //a to go forward
        mInputMap[MovementFlags::UP] = buttonReleased;
		break;
	case SDL_CONTROLLER_BUTTON_B://b to brake
        mInputMap[MovementFlags::DOWN] = buttonReleased;
		break;
	case SDL_CONTROLLER_BUTTON_X:
        mInputMap[MovementFlags::ACTION] = buttonReleased;
		break;
	case SDL_CONTROLLER_BUTTON_Y:
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		break;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		break;
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		break;
	case SDL_CONTROLLER_BUTTON_START:
        mInputMap[MovementFlags::ENTER] = buttonReleased;
		break;
	default:
		return;
    }

}

void Input::HandleJoystick() {

    //if(mEvent.caxis.value < -JOYSTICK_DEAD_ZONE || mEvent.caxis.value > JOYSTICK_DEAD_ZONE)

    //X axis motion

    if (mEvent.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
    {
        //Left of dead zone
        if (mEvent.caxis.value < -JOYSTICK_DEAD_ZONE)
        {
            mInputMap[MovementFlags::LEFT] = false;
            mInputMap[MovementFlags::RIGHT] = true;
        }
        //Right of dead zone
        if (mEvent.caxis.value > JOYSTICK_DEAD_ZONE)
        {
            mInputMap[MovementFlags::RIGHT] = false;
            mInputMap[MovementFlags::LEFT] = true;
        }
        //no joystick movement at all, dead center
        if (mEvent.caxis.value <= JOYSTICK_DEAD_ZONE && mEvent.caxis.value >= -JOYSTICK_DEAD_ZONE) {
            mInputMap[MovementFlags::LEFT] = true;
            mInputMap[MovementFlags::RIGHT] = true;
        }
        

    } //Y axis motion

    else if (mEvent.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
    {
        //Below dead zone
        if (mEvent.caxis.value < -JOYSTICK_DEAD_ZONE)
        {
            //yDir = -1;
        }
        //Above dead zone
        else if (mEvent.caxis.value > JOYSTICK_DEAD_ZONE)
        {
            //yDir = 1;
        }
        else
        {
           //yDir = 0;
        }

    }
    else if (mEvent.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
    }
    else if (mEvent.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
    }
    else {
    
    }

}

void Input::HandleMouse()
{
    if (mEvent.type == SDL_MOUSEBUTTONDOWN)
        mouseHeld = true;
    else
        mouseHeld = false;
}

void Input::HandleMouseMove()
{
    if (mouseHeld)
    {
        mouseX = mEvent.motion.x;
        mouseY = mEvent.motion.y;
    }
}

