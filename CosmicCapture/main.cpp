#include <string>
#include <memory>
#include <fmt/format.h>
#include <GL/glew.h>
#include <SDL/SDL.h>

#include "graphics/Window.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Model.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"

#include "./input.h"

#include "Physics.h"
#include "Camera.h"
#include "Render.h"

#include "ContactModification.h"

int main(int argc, char** args) {
	// Window Initialization
	const GLint width = 1280, height = 720;
	Window window("Cosmic Capture", width, height);
	const float aspect = static_cast<float>(width) / static_cast<float>(height);


	//physics
	Physics physics = Physics::Instance();
	const auto sCamera = std::make_shared<Camera>(PxVec3(5.0f, 5.0f, 5.0f), PxVec3(-0.6f, -0.2f, -0.7f), aspect);
	physics.Initialize();

	//input
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) { //initializing SDL with joystick support
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	//querying the number of available joysticks
	printf("%i joysticks were found.\n\n", SDL_NumJoysticks());


	Input input = Input(physics);

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

	//gameplay sample stuff------------------------
	Model dynamicBall("models/ball.ply", "textures/blue.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	Model staticWall("models/static_wall.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	Model flag("models/flag.ply", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	Model dropoffZone("models/dropoff_zone.ply", "textures/dropflaghere.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	//---------------------------------------------

	std::vector<Model> models;
	models.reserve(11); // Make space for 11 models without the need for copying
	models.push_back(std::move(wheel1));
	models.push_back(std::move(wheel2));
	models.push_back(std::move(wheel3));
	models.push_back(std::move(wheel4));
	models.push_back(std::move(wheel5));
	models.push_back(std::move(wheel6));
	models.push_back(std::move(body));
	models.push_back(std::move(dynamicBall));
	models.push_back(std::move(staticWall));
	models.push_back(std::move(flag));
	models.push_back(std::move(dropoffZone));
	

	//event handler;
	SDL_Event event;
	//main loop flag
	bool quit = false;

	// Loop until the user closes the window


	while (!quit) {
		
	
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
	SDL_JoystickClose(input.gGameController);
	input.gGameController = NULL;
	physics.CleanupPhysics();

	return 0;
}
