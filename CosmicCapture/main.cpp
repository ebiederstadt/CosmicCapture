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


# define M_PI  3.14159265358979323846

float angle = -0.25f;

int main(int argc, char** args) {
	// Window Initialization
	const GLint width = 1280, height = 720;
	Window window("Cosmic Capture", width, height);
	const float aspect = static_cast<float>(width) / static_cast<float>(height);

	//physics
	Physics physics = Physics::Instance();
	const auto sCamera = std::make_shared<Camera>(PxVec3(0.0f, 7.0f, -13.0f), PxVec3(-0.6f, -0.2f, -0.7f), aspect);
	physics.Initialize();

	Input input = Input();

	ShaderProgram shaderProgram("shaders/main.vert", "shaders/main.frag");
	shaderProgram.compile();

	// The arena model
	Model arena("models/basic_arena.ply", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);


	auto wheel1 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto wheel2 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto wheel3 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto wheel4 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto wheel5 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto wheel6 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);

	auto body = std::make_shared<Model>("models/cube.ply", "textures/camouflage.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
  //gameplay sample stuff------------------------
  auto dynamicBall = std::male_shared<Model>
  auto staticWall = std::male_shared<Model>
  auto flag = std::male_shared<Model>
  auto dropoffZone std::male_shared<Model>
  //---------------------------------------------
  
	std::vector<std::shared_ptr<Model>> models;
	models.reserve(11); // Make space for 11 models without the need for copying
	models.push_back(wheel1);
	models.push_back(wheel2);
	models.push_back(wheel3);
	models.push_back(wheel4);
	models.push_back(wheel5);
	models.push_back(wheel6);
	models.push_back(body);
  models.push_back(dynamicBall);
  models.push_back(staticWall);
  models.push_back(flag);
  models.push_back(dropoffZone);


	

	//event handler;
	SDL_Event event;
	//main loop flag
	bool quit = false;

	// Loop until the user closes the window


	while (!quit) {

		quit = input.HandleInput();

		
	
		if (SDL_NumJoysticks() < 1)
			printf("Warning: No joysticks connected!\n");
		else{
			//load joystic
			input.gGameController = SDL_JoystickOpen(0);
			if (input.gGameController == NULL) {
				printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
			}
		}
		// Input
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT)
				quit = true;
			else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
				input.HandleKeys(event);
			}
			else if (event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP) {
				input.HandleButtons(event);
			}
			else if (event.type == SDL_JOYAXISMOTION) {
				input.HandleJoystick(event);
			}
		}
		//this should probably be moved into physics at some point
		//should also assign keys to a list and iterate through that to see which keys trigger what event
		if (input.getDownUp()) {
			physics.stopBrakeMode();
			//physics.stopAccelerateForwardsMode();
		}
		else {
			physics.startBrakeMode();
			//physics.startAccelerateReverseMode();
		}
		if (input.getUpUp()) {
			physics.stopAccelerateForwardsMode();
		}
		else {
			physics.startAccelerateForwardsMode();
		}
		if (input.getRightUp()) { //for some reason the right/left controls are reversed so it has to be set this way (for now)
			physics.stopTurnHardLeftMode();
		}
		else {
			physics.startTurnHardLeftMode();
		}
		if (input.getLeftUp()) {
			physics.stopTurnHardRightMode();
		}
		else {
			physics.startTurnHardRightMode();
		}

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

		// Update camera
		sCamera->updateCamera(body->getModelMatrix());

		shaderProgram.use();

		// Draw arena
		arena.drawArena();

		auto counter = 1;
		for (auto& model : models)
		{
			model->draw(modelMatrices[counter]);
			++counter;
		}

		ImGui::Begin("Framerate Counter!");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Camera Position");
		ImGui::SliderFloat("Camera angle", &angle, -2.0f * M_PI, 2.0f * M_PI);
		ImGui::End();

		Window::renderImGuiFrame();
		window.swap();
	}
	//cleanup
	physics.CleanupPhysics();

	return 0;
}
