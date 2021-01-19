#include <fmt/format.h>
#include <SDL/SDL.h>
#include <GL/glew.h>


int main(int argc, char** args) {

	GLuint gProgamID = 0;

	// Pointers to our window and surface
	SDL_Surface* winSurface = nullptr;
	SDL_Window* window = nullptr;

	// Initialize SDL. SDL_Init will return -1 if it fails.
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fmt::print("Error initializing SDL : {} ", SDL_GetError());
		return 1;
	}

	// Create our window
	window = SDL_CreateWindow("Cosmic Capture", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);

	// Make sure creating the window succeeded
	if (!window) {
		fmt::print("Error creating window: {}", SDL_GetError());
		return 1;
	}

	// Get the surface from the window
	winSurface = SDL_GetWindowSurface(window);

	// Make sure getting the surface succeeded
	if (!winSurface) {
		fmt::print("Error getting surface: ", SDL_GetError());
		return 1;
	}

	// Fill the window with a white rectangle
	SDL_FillRect(winSurface, NULL, SDL_MapRGB(winSurface->format, 255, 255, 255));

	// Update the window display
	SDL_UpdateWindowSurface(window);

	// Destroy the window. This will also destroy the surface
	SDL_DestroyWindow(window);

	// Quit SDL
	SDL_Quit();

	// End the program
	return 0;
}
