#include <string>
#include <fmt/format.h>

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


int main(int argc, char** args) {

	// TODO: Make the window resizable
	GLint width = 800, height = 600;

	// Initialize SDL. SDL_Init will return -1 if it fails.
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fmt::print("Error initializing SDL : {} ", SDL_GetError());
		return 1;
	}

	// Use modren OpenGL (deprecated functions disabled)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Create the window and the context to use OpenGL
	SDL_Window* window = SDL_CreateWindow("Cosmic Capture", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_OPENGL);
	if (!window) {
		fmt::print("Error creating window: {}", SDL_GetError());
		return 1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (GLEW_OK != glewInit()) {
		fmt::print("Failed to initialize GLEW!");
		return 1;
	}

	glViewport(0, 0, width, height);

	SDL_Event windowEvent;

	// Loop untill the user closes the window
	while (true) {
		if (SDL_PollEvent(&windowEvent)) {
			if (windowEvent.type == SDL_QUIT) break;
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
