#include <string>
#include <fmt/format.h>

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


// TODO: Shaders should be stored in files :)
const std::string vertexSource = R"glsl(
	#version 330 core

	layout (location = 0) in vec3 aPos;
	void main()
	{
		gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	}
)glsl";

const std::string fragmentSource = R"glsl(
	#version 330 core

	out vec4 fragColor;

	void main()
	{
		fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);	
	}
)glsl";

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

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};


	// Vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* cVertexSource = vertexSource.c_str();
	glShaderSource(vertexShader, 1, &cVertexSource, nullptr);
	glCompileShader(vertexShader);

	// Fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* cFragmentSource = fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &cFragmentSource, nullptr);
	glCompileShader(fragmentShader);

	// Link Shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		fmt::print("ERROR::SHADER::PROGRAM::LINKING_FAILED\n {}", infoLog);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Vertex buffer, vertex array
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Loop until the user closes the window
	while (true) {
		if (SDL_PollEvent(&windowEvent)) {
			if (windowEvent.type == SDL_QUIT) break;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
