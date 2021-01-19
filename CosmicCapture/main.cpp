#include <fmt/format.h>

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


int main(int argc, char** args) {
	// TODO: Make the window resizable
	const GLint width = 1280, height = 720;

	// Initialize SDL. SDL_Init will return -1 if it fails.
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fmt::print("Error initializing SDL : {} ", SDL_GetError());
		return 1;
	}

	// Use modern OpenGL (deprecated functions disabled)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Create the window and the context to use OpenGL
	SDL_Window* window = SDL_CreateWindow("Cosmic Capture", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
	if (!window) {
		fmt::print("Error creating window: {}", SDL_GetError());
		return 1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);

	// Initialize GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fmt::print("Error initializing GLEW: {}", glewGetErrorString(err));
		return 1;
	}

	glViewport(0, 0, width, height);

	SDL_Event windowEvent;
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	// Loop until the user closes the window
	while (true) {
		if (SDL_PollEvent(&windowEvent)) {
			if (windowEvent.type == SDL_QUIT) break;
		}

		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderClear(renderer);

		//Render red filled quad
		SDL_Rect fillRect = { width / 4, height / 4, width / 2, height / 2 };
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderFillRect(renderer, &fillRect);

		SDL_RenderPresent(renderer);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
