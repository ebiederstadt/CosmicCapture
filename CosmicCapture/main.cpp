#include <string>
#include <memory>
#include <fmt/format.h>
#include <GL/glew.h>
#include <SDL/SDL.h>

#include "graphics/Window.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Model.h"
#include "imgui/imgui.h"

#include "./input.h"

#include "Physics.h"
#include "Camera.h"
#include "Render.h"


int main(int argc, char** args) {
	// Window Initialization
	const GLint width = 1280, height = 720;
	Window window("Cosmic Capture", width, height);
	const float aspect = static_cast<float>(width) / static_cast<float>(height);

	//physics
	Physics physics = Physics::Instance();
	const auto sCamera = std::make_shared<Camera>(PxVec3(5.0f, 5.0f, 5.0f), PxVec3(-0.6f, -0.2f, -0.7f), aspect);
	physics.Initialize();

	Input input = Input();

	ShaderProgram shaderProgram("shaders/main.vert", "shaders/main.frag");
	shaderProgram.compile();

	// Models (examples, please change)
	Model wheel1("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	Model wheel2("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	Model wheel3("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	Model wheel4("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	Model wheel5("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	Model wheel6("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);

	Model body("models/cube.ply", "textures/camouflage.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);

	// The arena model
	Model arena("models/basic_arena.ply", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);

	std::vector<Model> models;
	models.reserve(10); // Make space for 10 models without the need for copying
	models.push_back(std::move(wheel1));
	models.push_back(std::move(wheel2));
	models.push_back(std::move(wheel3));
	models.push_back(std::move(wheel4));
	models.push_back(std::move(wheel5));
	models.push_back(std::move(wheel6));
	models.push_back(std::move(body));

	//main loop flag
	bool quit = false;

	// Loop until the user closes the window
	while (!quit) {
		quit = input.HandleInput();

		// Physics simulation
		physics.processInput(input.getInputState());
		physics.stepPhysics();

		PxU32 nbActors = physics.gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		std::vector<PxMat44> modelMatrices;
		if (nbActors)
		{
			std::vector<PxRigidActor*> actors(nbActors);

			physics.gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
			modelMatrices = generateTransform(&actors[0], static_cast<PxU32>(actors.size()));
		}

		// Render
		window.startImGuiFrame();
		Window::clear();

		shaderProgram.use();

		// Draw arena
		arena.drawArena();

		auto counter = 1;
		for (auto& model : models)
		{
			model.draw(modelMatrices[counter]);
			++counter;
		}

		ImGui::Begin("Framerate Counter!");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		Window::renderImGuiFrame();
		window.swap();
	}
	//cleanup
	physics.CleanupPhysics();

	return 0;
}
