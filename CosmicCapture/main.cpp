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
#include "Colors.h"
#include "Vehicle.h"
#include "Flag.h"
#include "FlagDropoffZone.h"
#include "GameUI.h"
#include "PowerUpManager.h"

#include "OpponentInput.h"
#include "GridMarker.h"

#include "GlobalState.h"


#define M_PI  3.14159265358979323846

float angle = -0.25f;

int main(int argc, char** args)
{
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
	Model arena("models/untitled.obj", WHITE, sCamera);

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
	             SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = {1.0, 1.0, 1.0, 1.0, 1.0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set sampler aliases
	shaderProgram.use();

	auto shaderID = static_cast<unsigned int>(shaderProgram);
	auto samplerLoc = glGetUniformLocation(shaderID, "textureSampler");
	glUniform1i(samplerLoc, 0);
	samplerLoc = glGetUniformLocation(shaderID, "shadowMap");
	glUniform1i(samplerLoc, 1);


	//main loop flag
	bool quit = false;

	// Entities
	Vehicle car(sCamera, 0, "textures/blank.jpg");
	car.attachPhysics(physics);
	State::vehicles[0] = car.getVehicle();

	Vehicle opponentCar1(sCamera, 1, "textures/blue.jpg");
	opponentCar1.attachPhysics(physics);
	State::vehicles[1] = opponentCar1.getVehicle();
	
	Vehicle opponentCar2(sCamera, 2, "textures/pink.jpg");
	opponentCar2.attachPhysics(physics);
	State::vehicles[2] = opponentCar2.getVehicle();
	
	Vehicle opponentCar3(sCamera, 3, "textures/green.jpg");
	opponentCar3.attachPhysics(physics);
	State::vehicles[3] = opponentCar3.getVehicle();
	
	Flag flag(sCamera);
	flag.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone0(sCamera, 0);
	flagDropoffZone0.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone1(sCamera, 1);
	flagDropoffZone1.attachPhysics(physics);

	// setup audio
	AudioEngine soundSystem = AudioEngine();
	soundSystem.initialize();
	soundSystem.initializeBuffers();
	AudioInstance music = soundSystem.createInstance(audioConstants::SOUND_FILE_MAIN_TRACK);
	music.loop();
	music.playSound();
	//AudioInstance engine = soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE);
	//engine.loop();
	//engine.playSound();

	FlagDropoffZone flagDropoffZone2(sCamera, 2);
	flagDropoffZone2.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone3(sCamera, 3);
	flagDropoffZone3.attachPhysics(physics);

	std::vector<Entity*> entities;
	entities.push_back(&car);
	entities.push_back(&flag);
	entities.push_back(&flagDropoffZone0);
	entities.push_back(&flagDropoffZone1);
	entities.push_back(&flagDropoffZone2);
	entities.push_back(&flagDropoffZone3);
	entities.push_back(&opponentCar1);
	entities.push_back(&opponentCar2);
	entities.push_back(&opponentCar3);

	PowerUpManager powerUpManager(sCamera, physics);

	//GRID VISUALS TO HELP ME MAKE AI----------------------------------------
	//PxVec3 position1(100.f, 2.0f, 100.0f);
	//GridMarker gm1(shaderProgram, sCamera, position1);
	//gm1.attachPhysics(physics);
	//entities.push_back(&gm1);	
	//GRID VISUALS TO HELP ME MAKE AI----------------------------------------
	opponentBrains.updatePath(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
	//get Initial path
	std::pair<int, int> tempOld = State::lastPos;
	std::pair<int, int> tempNew = State::lastPos;
	int aiStuffCounter = 0;
	int stuckCount = 0; //count how many frames a player has been in the same grid coords
	int reverseCounter = 0;
	bool reversing = false;

	GameUI gameUI;
	bool gameStarted = false;

	std::map<MovementFlags, bool> inputState;

	auto preLoop = [&]()
	{
		gameUI.renderMenu();
		if (!inputState[MovementFlags::ENTER])
			gameStarted = true;
	};

	auto mainLoop = [&]()
	{
		// Physics simulation
		// Repeat for all vehicles eventually...
		car.processInput(inputState);

		powerUpManager.pickup(sCamera, physics);
		// TODO: Make it so that all players can use powerups
		powerUpManager.use(physics, inputState, 0);

		//forgive me--------------------
		PxVec3 target;
		if (State::targetReached)
		{
			target = State::flagDropoffBoxes[3]->getGlobalPose().p;
		}
		else
		{
			target = State::flagBody->getGlobalPose().p;
		}

		opponentBrains.updatePath(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p, target);

		std::map<MovementFlags, bool> command;
		command = opponentBrains.getInput(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p,
		                                  opponentCar3.mGeometry->getModelMatrix().column2.getXYZ());

		tempNew = State::lastPos;
		if (tempNew == tempOld && !reversing)
		{
			stuckCount++;
			if (stuckCount > 2000)
			{
				reversing = true;
				stuckCount = 0;
			}
		}
		else
			stuckCount = 0;
		tempOld = State::lastPos;
		if (reversing)
		{
			command[MovementFlags::LEFT] = true;
			command[MovementFlags::RIGHT] = true;
			command[MovementFlags::DOWN] = false;
			command[MovementFlags::UP] = true;
			reverseCounter++;
			if (reverseCounter > 850)
			{
				reversing = false;
				reverseCounter = 0;
			}
		}

		if (aiStuffCounter % 11 == 0)
			command[MovementFlags::UP] = true;
		opponentCar3.processInput(command);
		aiStuffCounter++;
		//------------------------------*/

		for (const auto& entity : entities)
			entity->simulate(physics);

		powerUpManager.simulate(physics);

		physics.stepPhysics();

		// Update camera
		sCamera->updateCamera(car.mGeometry->getModelMatrix());

		shaderProgram.use();

		// first render to depth map ---------------
		simpleDepthShader.use();

		/* Start of stuff to move to the--------------------------------*/
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// First pass
		arena.draw(simpleDepthShader, true);

		for (const auto& entity : entities)
			entity->draw(physics, simpleDepthShader, true);

		powerUpManager.draw(physics, simpleDepthShader, true);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Now standard rendering -----------------

		shaderProgram.use();

		float near_plane = 200.f, far_plane = 600.f;

		auto nearLoc = glGetUniformLocation(shaderID, "near_plane");
		glUniform1f(nearLoc, near_plane);

		auto farLoc = glGetUniformLocation(shaderID, "far_plane");
		glUniform1f(farLoc, far_plane);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glActiveTexture(GL_TEXTURE0);

		// Second pass
		arena.draw(shaderProgram, false);

		for (const auto& entity : entities)
			entity->draw(physics, shaderProgram, false);

		powerUpManager.draw(physics, shaderProgram, false);

		if (!State::flagPickedUpBy[0])
			gameUI.setCompassDirection(car.mGeometry->getModelMatrix(), flag.mGeometry->getModelMatrix());
		else if (State::flagPickedUpBy[0])
			gameUI.setCompassDirection(car.mGeometry->getModelMatrix(), State::flagDropoffBoxes[0]->getGlobalPose().p);
		gameUI.render();

		//player pos for testing
		//PxVec3 playerPosition = car.getVehicle()->getRigidDynamicActor()->getGlobalPose().p;
		//PxVec3 playerDir = car.mGeometry->getModelMatrix().column2.getXYZ();
		//int xIndex = (int)((playerPosition.x + 100.f) / 10.f);
		//int zIndex = (int)((playerPosition.z + 100.f) / 10.f);;
		//int dir = opponentBrains.getOrientation(playerDir);
		//printf("%f, %f, %f -- %f, %f, %f\n", playerPosition.x, playerPosition.y, playerPosition.z, playerDir.x, playerDir.y, playerDir.z);
		//printf("Coordinates: %f, %f, %f -- %d, %d. DirVector: x: %f, z: %f, dir: %d\n", playerPosition.x, playerPosition.y, playerPosition.z, xIndex, zIndex, playerDir.x, playerDir.z, dir);
		//printf("%d\n", State::worldGrid[17][6]);

		ImGui::Text("Camera Position");
		ImGui::SliderFloat("Camera angle", &angle, -2.0f * M_PI, 2.0f * M_PI);

		// Game State information
		if (State::scores[0] == 3)
		{
			ImGui::Text("You win");
			if (ImGui::Button("Restart?"))
			{
				for (int& score : State::scores)
					score = 0;
			}
		}
		else if (State::scores[1] == 3)
		{
			ImGui::Text("Opponent 1 wins");
			if (ImGui::Button("Restart?"))
			{
				for (int& score : State::scores)
					score = 0;
			}
		}
		else if (State::scores[2] == 3)
		{
			ImGui::Text("Opponent 2 wins");
			if (ImGui::Button("Restart?"))
			{
				for (int& score : State::scores)
					score = 0;
			}
		}
		else if (State::scores[3] == 3)
		{
			ImGui::Text("Opponent 3 wins");
			if (ImGui::Button("Restart?"))
			{
				for (int& score : State::scores)
					score = 0;
			}
		}

	};
	
	// Loop until the user closes the window
	while (!quit)
	{
		quit = input.HandleInput();
		inputState = input.getInputState();

		// Render
		window.startImGuiFrame();
		Window::clear();

		ImGui::Begin("Framerate Counter!");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		if (!gameStarted)
			preLoop();
		else
			mainLoop();

		ImGui::End();

		Window::renderImGuiFrame();
		window.swap();
	}
	//cleanup
	for (const auto& entity : entities)
		entity->cleanUpPhysics();
	powerUpManager.cleanUp();
	physics.CleanupPhysics();
	soundSystem.killSources();

	return 0;
}
