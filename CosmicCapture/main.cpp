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

#include "./physics/ContactReportCallback.h"

#include "Camera.h"
#include "Colors.h"
#include "Vehicle.h"
#include "Flag.h"
#include "FlagDropoffZone.h"
#include "GameUI.h"
#include "DoorSwitchZone.h"
#include "PowerUpManager.h"


#include "OpponentInput.h"
#include "GridMarker.h"
#include "InvisibleBarrier.h"

#include "GlobalState.h"


#define M_PI  3.14159265358979323846

float angle = -0.25f;


void initializeGridCenterCoords() {
	float flatOffset = 4.f; //TUNING POINT
	float diagonalOffset = 1.f; //TUNING POINT
	bool shifted = false;
	for (int i = 0; i < 36; i++) {
		for (int j = 0; j < 36; j++) {
			State::worldGridCenterCoords[i][j].first = i * 10.f - 180.f + 5.f;
			State::worldGridCenterCoords[i][j].second = j * 10.f - 180.f + 5.f;
			shifted = false;
			if ((i + 1 < 36) && (i - 1 >= 0) && (j + 1 < 36) && (j - 1 >= 0)) {
				if (State::worldGrid[i + 1][j] == 0) {
					State::worldGridCenterCoords[i][j].first -= flatOffset;
					shifted = true;
				}
				if (State::worldGrid[i - 1][j] == 0) {
					State::worldGridCenterCoords[i][j].first += flatOffset;
					shifted = true;
				}
				if (State::worldGrid[i][j + 1] == 0) {
					State::worldGridCenterCoords[i][j].second -= flatOffset;
					shifted = true;
				}
				if (State::worldGrid[i][j - 1] == 0) {
					State::worldGridCenterCoords[i][j].second += flatOffset;
					shifted = true;
				}

				if (shifted) continue;

				if (State::worldGrid[i - 1][j - 1] == 0) {
					State::worldGridCenterCoords[i][j].first += diagonalOffset;
					State::worldGridCenterCoords[i][j].second += diagonalOffset;
				}
				if (State::worldGrid[i + 1][j + 1] == 0) {
					State::worldGridCenterCoords[i][j].first -= diagonalOffset;
					State::worldGridCenterCoords[i][j].second -= diagonalOffset;
				}
				if (State::worldGrid[i + 1][j - 1] == 0) {
					State::worldGridCenterCoords[i][j].first -= diagonalOffset;
					State::worldGridCenterCoords[i][j].second += diagonalOffset;
				}
				if (State::worldGrid[i - 1][j + 1] == 0) {
					State::worldGridCenterCoords[i][j].first += diagonalOffset;
					State::worldGridCenterCoords[i][j].second -= diagonalOffset;
				}
				
			}

		}
	}
}
	
void updateWorldGridArena1() {
	State::worldGrid[12][17] = 0;
	State::worldGrid[12][18] = 0;
	State::worldGrid[28][17] = 0;
	State::worldGrid[28][18] = 0;
	State::worldGrid[7][1] = 0;
	State::worldGrid[7][2] = 0;
	State::worldGrid[29][1] = 0;
	State::worldGrid[29][2] = 0;
	State::worldGrid[7][33] = 0;
	State::worldGrid[7][34] = 0;
	State::worldGrid[29][33] = 0;
	State::worldGrid[29][34] = 0;

	State::worldGrid[17][12] = 1;
	State::worldGrid[18][12] = 1;
	State::worldGrid[17][28] = 1;
	State::worldGrid[18][28] = 1;
	State::worldGrid[1][7] = 1;
	State::worldGrid[2][7] = 1;
	State::worldGrid[1][29] = 1;
	State::worldGrid[2][29] = 1;
	State::worldGrid[33][7] = 1;
	State::worldGrid[34][7] = 1;
	State::worldGrid[33][29] = 1;
	State::worldGrid[34][29] = 1;

}
void updateWorldGridArena2() {
	State::worldGrid[12][17] = 1;
	State::worldGrid[12][18] = 1;
	State::worldGrid[23][17] = 1;
	State::worldGrid[23][13] = 1;
	State::worldGrid[7][1] = 1;
	State::worldGrid[7][2] = 1;
	State::worldGrid[29][1] = 1;
	State::worldGrid[29][2] = 1;
	State::worldGrid[7][33] = 1;
	State::worldGrid[7][34] = 1;
	State::worldGrid[29][33] = 1;
	State::worldGrid[29][34] = 1;

	State::worldGrid[17][12] = 0;
	State::worldGrid[18][12] = 0;
	State::worldGrid[17][23] = 0;
	State::worldGrid[18][23] = 0;
	State::worldGrid[1][7] = 0;
	State::worldGrid[2][7] = 0;
	State::worldGrid[1][29] = 0;
	State::worldGrid[2][29] = 0;
	State::worldGrid[33][7] = 0;
	State::worldGrid[34][7] = 0;
	State::worldGrid[33][29] = 0;
	State::worldGrid[34][29] = 0;
}

int main(int argc, char** args) {
	initializeGridCenterCoords();
	updateWorldGridArena2();
	// Window Initialization
	const GLint width = 1280, height = 720;
	Window window("Cosmic Capture", width, height);
	const float aspect = static_cast<float>(width) / static_cast<float>(height);

	//physics
	Physics physics = Physics::Instance();
	const auto sCamera = std::make_shared<Camera>(PxVec3(0.0f, 7.0f, -13.0f), PxVec3(-0.6f, -0.2f, -0.7f), aspect);
	physics.Initialize();

	Input input = Input();
	OpponentInput opponentBrains[3];
	opponentBrains[0].setPlayerNum(1);
	opponentBrains[1].setPlayerNum(2);
	opponentBrains[2].setPlayerNum(3);


	ShaderProgram shaderProgram("shaders/main.vert", "shaders/main.frag");
	shaderProgram.compile();

	ShaderProgram simpleDepthShader("shaders/simple.vert", "shaders/simple.frag");
	simpleDepthShader.compile();

	// The arena model

	Model arena("models/arena.obj", "textures/arena_texture.jpg", sCamera, GL_DYNAMIC_DRAW);
	Model walls("models/walls.obj", "textures/walls_texture.jpg", sCamera, GL_DYNAMIC_DRAW);
	Model redGates("models/red_gates.obj", "textures/blank.jpg", sCamera, GL_DYNAMIC_DRAW);
	Model blueGates("models/blue_gates.obj", "textures/blank.jpg", sCamera, GL_DYNAMIC_DRAW);
	Model skybox("models/skybox.obj", "textures/stars.jpg", sCamera, GL_DYNAMIC_DRAW);
	// Shadow setup start ---------------------------------------------------------------------

	// Configure depth map FBO
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const unsigned int SHADOW_WIDTH = 1024 * 15, SHADOW_HEIGHT = 1024 * 15;

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

	Vehicle car(sCamera, 0, "models/car_body.obj", "textures/car_body_texture.jpg", "textures/green_tire_texture.jpg");
	//Vehicle car(shaderProgram, sCamera, 0, "textures/test_texture.png");

	car.attachPhysics(physics);
	State::vehicles[0] = car.getVehicle();


	Vehicle opponentCar1(sCamera, 1, "models/blueCar.obj", "textures/blue_car_texture.jpg", "textures/blue_tire_texture.jpg");
	opponentCar1.attachPhysics(physics);
	State::vehicles[1] = opponentCar1.getVehicle();
	opponentBrains[0].attachVehicle(opponentCar1.getVehicle());
	
	Vehicle opponentCar2(sCamera, 2, "models/redCar.obj", "textures/red_car_texture.jpg", "textures/red_tire_texture.jpg");
	opponentCar2.attachPhysics(physics);
	opponentBrains[1].attachVehicle(opponentCar2.getVehicle());
	
	State::vehicles[2] = opponentCar2.getVehicle();
	Vehicle opponentCar3(sCamera, 3, "models/yellowCar.obj", "textures/yellow_car_texture.jpg", "textures/yellow_tire_texture.jpg");

	opponentCar3.attachPhysics(physics);
	State::vehicles[3] = opponentCar3.getVehicle();
	opponentBrains[2].attachVehicle(opponentCar3.getVehicle());
	
	Flag flag(sCamera);
	

	flag.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone0(sCamera, 0);
	flagDropoffZone0.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone1(sCamera, 1);
	flagDropoffZone1.attachPhysics(physics);
	
	FlagDropoffZone flagDropoffZone2(sCamera, 2);
	flagDropoffZone2.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone3(sCamera, 3);
	flagDropoffZone3.attachPhysics(physics);


	DoorSwitchZone doorSwitchZone(sCamera);
	doorSwitchZone.attachPhysics(physics);

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

	// setup audio
	Audio::soundSystem.initialize();
	Audio::soundSystem.initializeBuffers();
	Audio::music = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_MAIN_TRACK);
	Audio::music.loop();
	Audio::music.setVolume(0.35f);
	Audio::music.playSound();
	Audio::engine = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE);
	Audio::engine.loop();
	Audio::engine.setVolume(0.3f);
	Audio::engine.playSound();
	Audio::collision = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_COLLISION);
	Audio::projectile = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_PROJECTILE);
	Audio::flag_pickup = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_FLAG_PICKUP);
	Audio::projectile_pickup = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_PROJECTILE_PICKUP);
	Audio::spike_trap_pickup = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_SPIKE_TRAP_PICKUP);
	Audio::speed_boost_pickup = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_SPEED_BOOST_PICKUP);
	Audio::flag_return = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_FLAG_RETURN);
	Audio::speed_boost = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_SPEED_BOOST);



	entities.push_back(&doorSwitchZone);
	InvisibleBarrier barriers(sCamera, 0);
	barriers.attachPhysics(physics);
	entities.push_back(&barriers);



	for (int opponentNum = 1; opponentNum < 4; opponentNum++) {		
		opponentBrains[opponentNum - 1].updatePath(State::vehicles[opponentNum]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
	}
	int aiStuffCounter = 0;
	
	
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

		//used for respawning cars
		if (State::killCar0) {
			car.getVehicle()->getRigidDynamicActor()->release();
			car.attachPhysics(physics);
			std::cout << "Respawning player" << std::endl;
			State::killCar0 = false;
		}
		if (State::killCar1) {
			opponentCar1.getVehicle()->getRigidDynamicActor()->release();
			opponentCar1.attachPhysics(physics);
			std::cout << "Respawning opponent 1" << std::endl;
			State::killCar1 = false;
		}
		if (State::killCar2) {
			opponentCar2.getVehicle()->getRigidDynamicActor()->release();
			opponentCar2.attachPhysics(physics);
			std::cout << "Respawning opponent 2" << std::endl;
			State::killCar2 = false;
		}
		if (State::killCar3) {
			opponentCar3.getVehicle()->getRigidDynamicActor()->release();
			opponentCar3.attachPhysics(physics);
			std::cout << "Respawning opponent 3" << std::endl;
			State::killCar3 = false;
		}

		powerUpManager.pickup(sCamera, physics);
		// TODO: Make it so that all players can use powerups
		powerUpManager.use(physics, inputState, 0);


		//arena door switch
		if (State::arenaSwitch && State::arenaSwitchReady) {
			//need undraw code here
			if (State::blueArena) {
				updateWorldGridArena1();
				physics.generateRedDoor(); //switch from blue doors to red
				State::redArena = true;
				State::blueArena = false;
				//draw red arena
				//Model arena("models/redArena.obj", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
				fmt::print("Button pressed, doors switching\n");
				fmt::print("Red arena loaded\n");
			}
			else if (State::redArena) {
				updateWorldGridArena2();
				physics.generateBlueDoor(); //switch from red doors to blue
				State::blueArena = true;
				State::redArena = false;
				//draw blue arena
				//Model arena("models/blueArena.obj", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
				fmt::print("Button pressed, doors switching\n");
				fmt::print("Blue arena loaded\n");
			}
			State::arenaSwitch = false;
			State::arenaSwitchReady = false;
			State::arenaTimer = 0;
		}
		if (State::arenaSwitch && !State::arenaSwitchReady) {
			State::arenaSwitch = false;
		}


		//forgive me--------------------
		if (aiStuffCounter % 3 == 0) { //stagger pathfinding on different frames
			if (State::flagPickedUpBy[1]) {
				opponentBrains[0].updatePath(State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().p, State::flagDropoffBoxes[1]->getGlobalPose().p);
			}
			else {
				opponentBrains[0].updatePath(State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}
		else if (aiStuffCounter % 3 == 1) {
			if (State::flagPickedUpBy[2]) {
				opponentBrains[1].updatePath(State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().p, State::flagDropoffBoxes[2]->getGlobalPose().p);
			}
			else {
				opponentBrains[1].updatePath(State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}
		else {
			if (State::flagPickedUpBy[3]) {
				opponentBrains[2].updatePath(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p, State::flagDropoffBoxes[3]->getGlobalPose().p);
			}
			else {
				opponentBrains[2].updatePath(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}
		opponentCar1.processInput(opponentBrains[0].getInput(State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().p, opponentCar1.mGeometry->getModelMatrix().column2.getXYZ()));
		opponentCar2.processInput(opponentBrains[1].getInput(State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().p, opponentCar2.mGeometry->getModelMatrix().column2.getXYZ()));
		opponentCar3.processInput(opponentBrains[2].getInput(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p, opponentCar3.mGeometry->getModelMatrix().column2.getXYZ()));

		aiStuffCounter++;
		//------------------------------*/

		for (const auto& entity : entities)
			entity->simulate(physics);

		powerUpManager.simulate(physics);

		physics.stepPhysics();

		// Update camera
		sCamera->updateCamera(car.mGeometry->getModelMatrix());

		//Update sound
		Audio::engine.setVolume(0.3f + 0.001f*car.getSpeed());
		printf("%f \n", car.getSpeed());

		shaderProgram.use();

		// first render to depth map ---------------
		simpleDepthShader.use();

		/* Start of stuff to move to the--------------------------------*/
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// First pass

		arena.draw(simpleDepthShader, true, 1);
		walls.draw(simpleDepthShader, true, 1);
		// don't include skybox in depth map

		if(State::redArena) redGates.draw(simpleDepthShader, true, 1);
		if(State::blueArena) blueGates.draw(simpleDepthShader, true, 1);

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

		arena.draw(shaderProgram, false, 1);
		walls.draw(shaderProgram, false, 1);
		skybox.draw(shaderProgram, false, 0);

		if (State::redArena) {
			redGates.draw(shaderProgram, false, 1);
		}
		if (State::blueArena) {
			blueGates.draw(shaderProgram, false, 1);
		}
		for (const auto& entity : entities)
			entity->draw(physics, shaderProgram, false);

		powerUpManager.draw(physics, shaderProgram, false);

		if (!State::flagPickedUpBy[0])
			gameUI.setCompassDirection(car.mGeometry->getModelMatrix(), flag.mGeometry->getModelMatrix());
		else if (State::flagPickedUpBy[0])
			gameUI.setCompassDirection(car.mGeometry->getModelMatrix(), State::flagDropoffBoxes[0]->getGlobalPose().p);
		gameUI.render();

		//scott's debugging prints----------------------------------------------------------------------------------------------
		//PxVec3 playerPosition = car.getVehicle()->getRigidDynamicActor()->getGlobalPose().p;
		//PxVec3 playerDir = car.mGeometry->getModelMatrix().column2.getXYZ();
		//PxVec3 playerToTarget = opponentBrains[0].getPlayerToTargetDir(playerDir, 0, State::flagBody->getGlobalPose().p);
		//int xIndex = (int)((playerPosition.x + 180.f) / 10.f);
		//int zIndex = (int)((playerPosition.z + 180.f) / 10.f);;
		//int dir = opponentBrains[1].getOrientation(playerDir);
		//printf("%f, %f, %f (%f) -- %f, %f, %f (%f)\n", playerDir.x, playerDir.y, playerDir.z, atan2(playerDir.z, playerDir.x), playerToTarget.x, playerToTarget.y, playerToTarget.z, atan2(playerToTarget.z, playerToTarget.x));
		//printf("Coordinates: %f, %f, %f -- %d, %d. DirVector: x: %f, z: %f, dir: %d\n", playerPosition.x, playerPosition.y, playerPosition.z, xIndex, zIndex, playerDir.x, playerDir.z, dir);
		//-----------------------------------------------------------------------------------------------------------------------

		ImGui::Text("Camera Position");
		ImGui::SliderFloat("Camera angle", &angle, -2.0f * M_PI, 2.0f * M_PI);
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
	Audio::soundSystem.killSources();

	return 0;
}
