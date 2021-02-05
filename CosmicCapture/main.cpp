#include <string>
#include <memory>
#include <fmt/format.h>
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "graphics/Window.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Model.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"

#include "./input.h"

#include "Physics.h"
#include "Camera.h"
#include "Render.h"
#include "graphics/GraphicsCamera.h"

//physics stuff
Camera* sCamera;
Physics& physics = Physics::Instance();
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);
	PX_UNUSED(key);
}
void motionCallback(int x, int y)
{
	sCamera->handleMotion(x, y);
}

void keyboardCallback(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);

	if (!sCamera->handleKey(key, x, y))
		keyPress(key, sCamera->getTransform());
}

void mouseCallback(int button, int state, int x, int y)
{
	sCamera->handleMouse(button, state, x, y);
}

void idleCallback()
{
	glutPostRedisplay();
}

void renderCallback()
{
	physics.stepPhysics();

	startRender(sCamera->getEye(), sCamera->getDir());

	PxU32 nbActors = physics.gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		physics.gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
		renderActors(&actors[0], static_cast<PxU32>(actors.size()), true);
	}

	finishRender();
}

void exitCallback(void)
{
	delete sCamera;
	physics.CleanupPhysics();
}

Input input;

int main(int argc, char** args) {
	// Window Initialization
	const GLint width = 1280, height = 720;
	Window window("Cosmic Capture", width, height);

	//physics
	sCamera = new Camera(PxVec3(10.0f, 10.0f, 10.0f), PxVec3(-0.6f, -0.2f, -0.7f));

	setupDefaultWindow("PhysX Snippet Vehicle4W");
	setupDefaultRenderState();

	glutIdleFunc(idleCallback);
	glutDisplayFunc(renderCallback);
	glutKeyboardFunc(keyboardCallback);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(motionCallback);
	motionCallback(0, 0);

	atexit(exitCallback);

	physics.Initialize();
	glutMainLoop();
	///////////////////////////////////////////////

	// Shaders are used 1+ times, and shared between all models that use them
	const auto shaderProgram = std::make_shared<ShaderProgram>("shaders/main.vert", "shaders/main.frag");

	// The camera is used once, and shared between all geometry
	const auto camera = std::make_shared<GraphicsCamera>();

	// Models
	Model monkey = Model("models/monkey.ply", "textures/camouflage.jpg", shaderProgram, camera);
	monkey.scale(0.5f);

	std::vector<Model> models;
	models.push_back(std::move(monkey));

	float angle = 0.01f;

	// Loop until the user closes the window
	while (true) {
		if (SDL_PollEvent(&window.event)) {
			if (window.event.type == SDL_QUIT)
				break;
		}
    
    input.HandleEvent(window.event);

		window.startImGuiFrame();
		Window::clear();

		models[0].rotateZ(angle);

		for (auto& model : models)
			model.draw();

		ImGui::Begin("Framerate Counter!");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		window.swap();
	}

	return 0;
}
