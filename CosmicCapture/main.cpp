#include <string>
#include <memory>
#include <fmt/format.h>
#include <GL/glew.h>
#include <SDL/SDL.h>

#include "graphics/Window.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Model.h"
#include "imgui/imgui.h"

#include "input.h"

#include "physics/Physics.h"
#include "Camera.h"
#include "Vehicle.h"
#include "Flag.h"
#include "Projectile.h"
#include "ProjectilePickupZone.h"


#define M_PI  3.14159265358979323846

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
	Model arena("models/basic_arena.ply", "textures/blank.jpg", shaderProgram, sCamera);


	//main loop flag
	bool quit = false;

	// Entities
	Vehicle car(shaderProgram, sCamera, 0);
	car.attachPhysics(physics);

	//Vehicle opponentCar1(shaderProgram, sCamera, 1);
	//opponentCar1.attachPhysics(physics);
	ProjectilePickupZone projPickupZone(shaderProgram, sCamera);
	projPickupZone.attachPhysics(physics);
	
	Flag flag(shaderProgram, sCamera);
	flag.attachPhysics(physics);
	flag.attachVehicle(car.getVehicle());

	std::vector<Entity*> entities;
	entities.push_back(&car);
	entities.push_back(&flag);	
	entities.push_back(&projPickupZone);
	//entities.push_back(&opponentCar1);

	//projectile prototype stuff----------------------
	Projectile testProj(shaderProgram, sCamera);
	//------------------------------------------------

	// Loop until the user closes the window
	while (!quit) {
		quit = input.HandleInput();

		// Physics simulation
		auto inputState = input.getInputState();
		

		// Repeat for all vehicles eventually...
		car.processInput(inputState);
		
		if (inputState[MovementFlags::ACTION] == false && State::projectilePickedUp) {
			testProj.attachVehicle(car.getVehicle());
			testProj.attachPhysics(physics);			
			entities.push_back(&testProj);
			
			State::projectilePickedUp = false;
		}

		/*//forgive me--------------------
		std::map<MovementFlags, bool> testInputMap;
		testInputMap[MovementFlags::LEFT] = true;
		testInputMap[MovementFlags::RIGHT] = true;
		testInputMap[MovementFlags::DOWN] = true;
		testInputMap[MovementFlags::UP] = true;
		opponentCar1.processInput(testInputMap);
		//------------------------------*/
		

		for (const auto& entity : entities)
			entity->simulate(physics);
		
		physics.stepPhysics();

		// Render
		window.startImGuiFrame();
		Window::clear();

		// Update camera
		sCamera->updateCamera(car.mGeometry->getModelMatrix());

		shaderProgram.use();

		// Draw arena
		arena.drawArena();

		for (const auto& entity : entities)
			entity->draw(physics);

		ImGui::Begin("Framerate Counter!");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Camera Position");
		ImGui::SliderFloat("Camera angle", &angle, -2.0f * M_PI, 2.0f * M_PI);
		ImGui::End();

		Window::renderImGuiFrame();
		window.swap();
	}
	//cleanup
	for (const auto& entity : entities)
		entity->cleanUpPhysics();
	physics.CleanupPhysics();

	return 0;
}
