#include "Window.h"

#include <fmt/format.h>
#include <GL/glew.h>

#include "../GlDebug.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl.h"
#include "../imgui/imgui_impl_opengl3.h"


Window::Window(const std::string& windowName, const int width, const int height)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_GAMECONTROLLER) == -1)
	{
		fmt::print("Failed to initialize SDL: {}\n", SDL_GetError());
		throw std::runtime_error("SDL_INIT failed");
	}
	fmt::print("{} joysticks were found.\n\n", SDL_NumJoysticks());
	
	// Create the window
	mWindow = std::unique_ptr<SDL_Window, WindowDeleter>(SDL_CreateWindow(
		windowName.c_str(), 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		width, height, 
		SDL_WINDOW_OPENGL
	));
	if (mWindow == nullptr)
	{
		fmt::print("Error creating window: {}", SDL_GetError());
		throw std::runtime_error("Failed to open the window");
	}

	// Use modern OpenGL (deprecated functions disabled)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Setup the OpenGL context
	mContext = SDL_GL_CreateContext(mWindow.get());
	SDL_GL_MakeCurrent(mWindow.get(), mContext);

	// Initialize GLEW
	const auto err = glewInit();
	if (GLEW_OK != err)
	{
		fmt::print("Error initializing GLEW: {}", glewGetErrorString(err));
		throw std::runtime_error("Failed to initialize GLEW");
	}

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);

	// Turn on openGL debugging messages
	// (if running in debug mode)
#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	GLDebug::enable();
#endif

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	const char* glsl_version = "#version 330 core";
	ImGui_ImplSDL2_InitForOpenGL(mWindow.get(), mContext);
	ImGui_ImplOpenGL3_Init(glsl_version);
}


Window::~Window()
{
	// ImGui Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// SDL cleanup
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow.get());
	SDL_Quit();
}

void Window::clear()
{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::startImGuiFrame() const
{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(mWindow.get());
		ImGui::NewFrame();
}

void Window::renderImGuiFrame()
{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
