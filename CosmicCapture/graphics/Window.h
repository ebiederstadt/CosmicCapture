// Note: This class is inspired, but not copied from the fall 2020 CPSC 453 OpenGL Boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton

#pragma once

#include <string>
#include <memory>
#include <SDL/SDL.h>


struct WindowDeleter 
{
	void operator() (SDL_Window* window) const 
	{
		SDL_DestroyWindow(window);
	}
};


class Window
{
public:
	Window(const std::string&, int, int);
	~Window();

	void swap() const { SDL_GL_SwapWindow(mWindow.get()); }
	static void clear();
	void startImGuiFrame() const;
	static void renderImGuiFrame();

private:
	std::unique_ptr<SDL_Window, WindowDeleter> mWindow;
	SDL_GLContext mContext;
};
