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
#include "FlagDropoffZone.h"
#include "Projectile.h"
#include "ProjectilePickupZone.h"

#include "OpponentInput.h"

#include "GlobalState.h"


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

	//initialize world grid temp ----------------------------
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 25; j++) {
			State::worldGrid[i][j] = 1;
		}
	}

	for (int i = 1; i < 25; i++) {
		State::worldGrid[i][13] = 0;
	}
	for (int i = 0; i < 24; i++) {
		State::worldGrid[i][16] = 0;
	}
	//-------------------------------------------------------
	
	Input input = Input();

	OpponentInput opponentBrains(1);


	ShaderProgram shaderProgram("shaders/main.vert", "shaders/main.frag");
	shaderProgram.compile();

	// The arena model
	Model arena("models/basic_arena.ply", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);


	//main loop flag
	bool quit = false;


	// Entities
	Vehicle car(shaderProgram, sCamera, 0, "textures/blank.jpg");
	car.attachPhysics(physics);
	State::vehicleRDs[0] = car.getVehicle()->getRigidDynamicActor();

	Vehicle opponentCar1(shaderProgram, sCamera, 1, "textures/blue.jpg");
	opponentCar1.attachPhysics(physics);
	State::vehicleRDs[1] = opponentCar1.getVehicle()->getRigidDynamicActor();

	Vehicle opponentCar2(shaderProgram, sCamera, 2, "textures/pink.jpg");
	opponentCar2.attachPhysics(physics);
	State::vehicleRDs[2] = opponentCar2.getVehicle()->getRigidDynamicActor();

	Vehicle opponentCar3(shaderProgram, sCamera, 3, "textures/green.jpg");
	opponentCar3.attachPhysics(physics);
	State::vehicleRDs[3] = opponentCar3.getVehicle()->getRigidDynamicActor();

	//projectile prototype stuff----------------------
	Projectile testProj(shaderProgram, sCamera);
	ProjectilePickupZone projPickupZone(shaderProgram, sCamera);
	projPickupZone.attachPhysics(physics);
	//------------------------------------------------
	
	Flag flag(shaderProgram, sCamera);
	flag.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone0(shaderProgram, sCamera, 0);
	flagDropoffZone0.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone1(shaderProgram, sCamera, 1);
	flagDropoffZone1.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone2(shaderProgram, sCamera, 2);
	flagDropoffZone2.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone3(shaderProgram, sCamera, 3);
	flagDropoffZone3.attachPhysics(physics);


	std::vector<Entity*> entities;
	entities.push_back(&car);
	entities.push_back(&flag);
	entities.push_back(&flagDropoffZone0);
	entities.push_back(&flagDropoffZone1);
	entities.push_back(&flagDropoffZone2);
	entities.push_back(&flagDropoffZone3);
	entities.push_back(&projPickupZone);
	entities.push_back(&opponentCar1);
	entities.push_back(&opponentCar2);
	entities.push_back(&opponentCar3);


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

		//forgive me--------------------
		//opponentCar1.processInput(opponentBrains.getInput());
		PxMat44 playerMat = car.mGeometry->getModelMatrix();
		PxVec3 playerDir = playerMat.column2.getXYZ();
		//PxMat44 op1Mat = opponentCar1.mGeometry->getModelMatrix();
		//PxMat44 op2Mat = opponentCar2.mGeometry->getModelMatrix();
		//PxMat44 op3Mat = opponentCar3.mGeometry->getModelMatrix();
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
		
		//player pos for testing
		//PxVec3 playerPosition = car.getVehicle()->getRigidDynamicActor()->getGlobalPose().p;
		//PxVec3 playerDir = car.getVehicle()->getRigidDynamicActor()->getLinearVelocity();
		//printf("%f, %f, %f -- %f, %f, %f\n", playerPosition.x, playerPosition.y, playerPosition.z, playerDir.x, playerDir.y, playerDir.z);
		printf("%f, %f, %f\n", playerDir.x, playerDir.y, playerDir.z);

		if (State::scores[0] == 3) {
			fmt::print("You win ");
		}
		else if (State::scores[1] == 3) {
			fmt::print("Opponent 1 wins");
		}
		else if (State::scores[2] == 3) {
			fmt::print("Opponent 2 wins");
		}
		else if (State::scores[3] == 3) {
			fmt::print("Opponent 3 wins");
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
	for (const auto& entity : entities)
		entity->cleanUpPhysics();
	physics.CleanupPhysics();

	return 0;
}
