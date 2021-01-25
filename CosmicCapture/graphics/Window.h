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
	Window(const std::string& windowName, int width, int height);
	~Window();

	void swap() const { SDL_GL_SwapWindow(mWindow.get()); }
	static void clear();
	void startImGuiFrame() const;


	SDL_Event event;

private:
	std::unique_ptr<SDL_Window, WindowDeleter> mWindow;
	SDL_GLContext mContext;
};
