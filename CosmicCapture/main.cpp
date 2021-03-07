#include <memory>
#include <fmt/format.h>
#include <GL/glew.h>
#include <SDL/SDL.h>

#include "graphics/Window.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Model.h"
#include "imgui/imgui.h"

#include "input.h"

#include "./audio/AudioEngine.h"

#include "Camera.h"
#include "Vehicle.h"
#include "Flag.h"
#include "FlagDropoffZone.h"
#include "Projectile.h"
#include "ProjectilePickupZone.h"
#include "SpeedBoost.h"
#include "SpeedBoostPickupZone.h"
#include "SpikeTrap.h"
#include "SpikeTrapPickupZone.h"

#include "OpponentInput.h"
#include "GridMarker.h"

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
	
	Input input = Input();

	OpponentInput opponentBrains(1);


	ShaderProgram shaderProgram("shaders/main.vert", "shaders/main.frag");
	shaderProgram.compile();

	ShaderProgram simpleDepthShader("shaders/simple.vert", "shaders/simple.frag");
	simpleDepthShader.compile();

	// The arena model
	Model arena("models/untitled.obj", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);

	// Shadow setup start ---------------------------------------------------------------------

// Configure depth map FBO
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const unsigned int SHADOW_WIDTH = 1024 * 4, SHADOW_HEIGHT = 1024 * 4;

	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set sampler aliases
	shaderProgram.use();

	auto mShaderID = static_cast<unsigned int>(shaderProgram);
	auto samplerLoc = glGetUniformLocation(mShaderID, "textureSampler");
	glUniform1i(samplerLoc, 0);
	samplerLoc = glGetUniformLocation(mShaderID, "shadowMap");
	glUniform1i(samplerLoc, 1);


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

	//speedboost powerup
	SpeedBoost testSpeedBoost(shaderProgram, sCamera);
	SpeedBoostPickupZone speedboostPickupZone(shaderProgram, sCamera);
	speedboostPickupZone.attachPhysics(physics);

	// Spike trap powerup
	SpikeTrap testSpikeTrap(shaderProgram, sCamera);
	SpikeTrapPickupZone spikeTrapPickupZone(shaderProgram, sCamera);
	spikeTrapPickupZone.attachPhysics(physics);
	
	Flag flag(shaderProgram, sCamera);
	flag.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone0(shaderProgram, sCamera, 0);
	flagDropoffZone0.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone1(shaderProgram, sCamera, 1);
	flagDropoffZone1.attachPhysics(physics);

	// setup audio
	AudioEngine soundSystem = AudioEngine();
	soundSystem.initialize();
	soundSystem.initializeBuffers();
	//AudioInstance music = soundSystem.createInstance(audioConstants::SOUND_FILE_MAIN_TRACK);
	//music.loop();
	 //music.playSound();
	//AudioInstance engine = soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE);
	//engine.loop();
	//engine.playSound();

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
	entities.push_back(&speedboostPickupZone);
	entities.push_back(&opponentCar1);
	entities.push_back(&opponentCar2);
	entities.push_back(&opponentCar3);
	entities.push_back(&spikeTrapPickupZone);

	//GRID VISUALS TO HELP ME MAKE AI----------------------------------------
	PxVec3 position1(100.f, 2.0f, 100.0f);
	GridMarker gm1(shaderProgram, sCamera, position1);
	gm1.attachPhysics(physics);
	entities.push_back(&gm1);	
	//GRID VISUALS TO HELP ME MAKE AI----------------------------------------
	//opponentBrains.updatePath(State::vehicleRDs[3]->getGlobalPose().p, State::flagBody->getGlobalPose().p); //get Initial path
	int counter = 0;

	// Loop until the user closes the window
	while (!quit) {
		quit = input.HandleInput();

		// Physics simulation
		auto inputState = input.getInputState();
		
		// Repeat for all vehicles eventually...
		car.processInput(inputState);

		if (State::spikeTrapPickedUp && testSpikeTrap.hasOwningVehicle())
			testSpikeTrap.processInput(inputState, physics);
		
		if (inputState[MovementFlags::ACTION] == false && State::projectilePickedUp) {
			testProj.attachVehicle(car.getVehicle());
			testProj.attachPhysics(physics);			
			entities.push_back(&testProj);
			
			State::projectilePickedUp = false;
		}

		if (inputState[MovementFlags::ACTION] == false && State::speedboostPickedUp) {
			testSpeedBoost.attachVehicle(car.getVehicle());
			testSpeedBoost.attachPhysics(physics);
			State::speedboostPickedUp = false;
		}

		// Pickup spike trap
		if (State::spikeTrapPickedUp && !testSpikeTrap.hasOwningVehicle()) {
			testSpikeTrap.attachOwningVehicle(car.getVehicle());
			entities.push_back(&testSpikeTrap);
		}

		// Run into spike trap
		if (State::spikeTrapInUse && !testSpikeTrap.hasAffectedVehicle())
		{
			switch (State::spikeTrapActingUpon)
			{
			case 0:
				testSpikeTrap.attachAffectedVehicle(car.getVehicle());
				break;
			case 1:
				testSpikeTrap.attachAffectedVehicle(opponentCar1.getVehicle());
				break;
			case 2:
				testSpikeTrap.attachAffectedVehicle(opponentCar2.getVehicle());
				break;
			case 3:
				testSpikeTrap.attachAffectedVehicle(opponentCar3.getVehicle());
				break;
			}
		}

		// Delete spike trap once it is finished
		if (State::spikeTrapFinished)
		{
			State::spikeTrapFinished = false;
			auto loc = std::find(entities.begin(), entities.end(), &testSpikeTrap);
			entities.erase(loc);
			testSpikeTrap.cleanUpPhysics();
		}
		
		//forgive me--------------------
		//if (counter % 10 == 0) {
		//	opponentBrains.updatePath(State::vehicleRDs[3]->getGlobalPose().p, State::flagBody->getGlobalPose().p);
		//}
		//opponentCar3.processInput(opponentBrains.getInput(State::vehicleRDs[3]->getGlobalPose().p, opponentCar3.mGeometry->getModelMatrix().column2.getXYZ()));
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

		auto counter = 1;

		// first render to depth map ---------------
		simpleDepthShader.use();

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// First pass
		arena.drawArena(simpleDepthShader, true, depthMapFBO);

		for (const auto& entity : entities)
			entity->draw(physics, simpleDepthShader, true, depthMapFBO);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		//glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Now standard rendering -----------------

		shaderProgram.use();

		float near_plane = 200.f, far_plane = 600.f;

		auto nearLoc = glGetUniformLocation(mShaderID, "near_plane");
		glUniform1f(nearLoc, near_plane);

		auto farLoc = glGetUniformLocation(mShaderID, "far_plane");
		glUniform1f(farLoc, far_plane);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glActiveTexture(GL_TEXTURE0);

		// Second pass
		arena.drawArena(shaderProgram, false, depthMap);

		for (const auto& entity : entities)
			entity->draw(physics, shaderProgram, false, depthMap);

		//player pos for testing
		PxVec3 playerPosition = car.getVehicle()->getRigidDynamicActor()->getGlobalPose().p;
		PxVec3 playerDir = car.mGeometry->getModelMatrix().column2.getXYZ();
		int xIndex = (int)((playerPosition.x + 100.f) / 10.f);
		int zIndex = (int)((playerPosition.z + 100.f) / 10.f);;
		int dir = opponentBrains.getOrientation(playerDir);
		//printf("%f, %f, %f -- %f, %f, %f\n", playerPosition.x, playerPosition.y, playerPosition.z, playerDir.x, playerDir.y, playerDir.z);
		printf("Coordinates: %f, %f, %f -- %d, %d. DirVector: x: %f, z: %f, dir: %d\n", playerPosition.x, playerPosition.y, playerPosition.z, xIndex, zIndex, playerDir.x, playerDir.z, dir);
		//printf("%d\n", State::worldGrid[17][6]);
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
	soundSystem.killSources();

	return 0;
}
