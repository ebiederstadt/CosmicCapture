#include <memory>
#include <GL/glew.h>
#include <SDL/SDL.h>

#include "graphics/Window.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Model.h"
#include "imgui/imgui.h"

#include "input.h"

#include "audio/GameAudio.h"

#include "Camera.h"
#include "Vehicle.h"
#include "Flag.h"
#include "FlagDropoffZone.h"
#include "GameUI.h"
#include "DoorManager.h"
#include "PowerUpManager.h"

#include "OpponentInput.h"
#include "InvisibleBarrier.h"

#include "GlobalState.h"
#include "Timer.h"
#include "physics/VehicleCreate.h"

float angle = -0.25f;
glm::vec2 g_scale = { 1.f, 1.f };
glm::vec2 g_pos = { 1.0f, 1.0f };
float scalingFactor = 3.0f;

int main(int, char**) {
	initializeGridCenterCoords();
	updateWorldGridArenaToRedArena();

	const GLint width = 1280, height = 720;
	Window window("Cosmic Capture", width, height);

	Physics physics = Physics::Instance();
	physics.Initialize();

	// Cameras for each of the players
	const float aspect = static_cast<float>(width) / static_cast<float>(height);
	Camera camera(aspect);
	Camera camera1(aspect);
	Camera camera2(aspect);
	Camera camera3(aspect);

	std::array<Camera*, 4> cameras = { &camera, &camera1, &camera2, &camera3 };

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
	Model arenaPlane("models/arena_plane.obj", "textures/arena_plane_texture.jpg");
	Model centerArea("models/center_area.obj", "textures/center_area_texture.jpg");
	Model dome("models/dome.obj", "textures/dome.jpg");
	Model innerWalls("models/inner_walls.obj", "textures/pillars_texture.jpg");
	Model walls("models/walls.obj", "textures/pillars_texture.jpg");
	Model redGates("models/red_gates.obj", "textures/red_gates.jpg");
	Model blueGates("models/blue_gates.obj", "textures/blue_gates.jpg");
	Model skybox("models/skybox.obj", "textures/stars.jpg");

	// Shadow setup start ---------------------------------------------------------------------

	// Configure depth map FBO
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const unsigned int SHADOW_WIDTH = 1024 * 12, SHADOW_HEIGHT = 1024 * 12;

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


	shaderProgram.setInt("textureSampler", 0);
	shaderProgram.setInt("shadowMap", 1);

	//main loop flag
	bool quit = false;

	// Entities
	Vehicle car(0, "models/car_body.obj", "textures/car_body_texture.jpg", "textures/green_tire_texture.jpg");

	car.attachPhysics(physics);
	State::vehicles[0] = car.getVehicle();

	Vehicle opponentCar1(1, "models/blueCar.obj", "textures/blue_car_texture.jpg", "textures/blue_tire_texture.jpg");
	opponentCar1.attachPhysics(physics);
	State::vehicles[1] = opponentCar1.getVehicle();
	opponentBrains[0].attachVehicle(opponentCar1.getVehicle());

	Vehicle opponentCar2(2, "models/redCar.obj", "textures/red_car_texture.jpg", "textures/red_tire_texture.jpg");
	opponentCar2.attachPhysics(physics);
	opponentBrains[1].attachVehicle(opponentCar2.getVehicle());
	State::vehicles[2] = opponentCar2.getVehicle();

	Vehicle opponentCar3(3, "models/yellowCar.obj", "textures/yellow_car_texture.jpg", "textures/yellow_tire_texture.jpg");
	opponentCar3.attachPhysics(physics);
	State::vehicles[3] = opponentCar3.getVehicle();
	opponentBrains[2].attachVehicle(opponentCar3.getVehicle());

	std::array<Vehicle*, 4> cars = { &car, &opponentCar1, &opponentCar2, &opponentCar3 };

	Flag flag;
	flag.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone0(0);
	flagDropoffZone0.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone1(1);
	flagDropoffZone1.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone2(2);
	flagDropoffZone2.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone3(3);
	flagDropoffZone3.attachPhysics(physics);

	DoorManager doorManager;

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

	PowerUpManager powerUpManager(physics);

	GameAudio::initialize();

	InvisibleBarrier barriers(0);
	barriers.attachPhysics(physics);
	entities.push_back(&barriers);

	for (int opponentNum = 1; opponentNum < 4; opponentNum++) {
		opponentBrains[opponentNum - 1].updatePath(State::vehicles[opponentNum]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
	}
	int aiStuffCounter = 0;


	GameUI gameUI;
	bool gameStarted = false;
	bool showControls = false;
	bool playersSelected = false;
	bool gameFinished = false;

	InputInfo* info;

	// Frame rate control
	Timer fpsTimer;
	Timer capTimer;
	int countedFrames = 0;
	fpsTimer.start();
	constexpr int SCREEN_FPS = 60;
	constexpr int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

	auto processVehicleInput = [&input, &info](Vehicle& v)
	{
		if (v.useKeyboard)
			v.processInput(*info);
		else if (v.useController)
		{
			const InputInfo* controllerInfo = input.getInfo(v.controllerNumber);
			v.processInput(*controllerInfo);
		}
	};

	auto processPowerupInput = [&](Vehicle& v, int playerNum)
	{
		if (v.useKeyboard)
			powerUpManager.use(physics, *info, playerNum);
		else if (v.useController)
		{
			const InputInfo* controllerInfo = input.getInfo(v.controllerNumber);
			powerUpManager.use(physics, *controllerInfo, playerNum);
		}
	};

	// Display the logo and wait for the user to enter into the game
	auto preLoop = [&]()
	{
		const auto controllerInfo= input.getAllControllerInfo();
		bool controllerPressed = false;
		bool controls = false;
		for (auto& [id, c_info] : *controllerInfo)
		{
			if (c_info.inputReleased(MovementFlags::ENTER))
				controllerPressed = true;
			if (c_info.inputReleased(MovementFlags::OTHER_ACTION))
			{
				fmt::print("pressed back\n");
				controls = true;
			}
		}
		gameUI.renderMenu();
		if (!info->inputState[MovementFlags::ENTER] || controllerPressed)
			gameStarted = true;
		if (!info->inputState[MovementFlags::OTHER_ACTION] || controls)
			showControls = true;
	};

	auto controlLoop = [&]()
	{
		auto* const controllerInfo = input.getAllControllerInfo();
		bool returnToGame = false;
		for (auto& [id, c_info] : *controllerInfo)
		{
			if (c_info.inputReleased(MovementFlags::ENTER))
				returnToGame = true;
		}
		
		gameUI.renderControls();

		// Continue to show the controls until enter is pressed (either on the gamepad or on the keyboard)
		showControls = !info->inputState[MovementFlags::ENTER] || !returnToGame;
	};

	auto render = [&](int x, int y, int width, int height, int playerNum, bool isReversing = false)
	{

		// Engine for non-first players
		if (!GameAudio::isSoundPlaying(GameSounds::ENGINE2) && State::numHumanPlayers > 1)
			GameAudio::play(GameSounds::ENGINE2);

		// Engine for non-first players
		if (!GameAudio::isSoundPlaying(GameSounds::ENGINE3) && State::numHumanPlayers > 2)
			GameAudio::play(GameSounds::ENGINE3);

		// Engine for non-first players
		if (!GameAudio::isSoundPlaying(GameSounds::ENGINE4) && State::numHumanPlayers > 3)
			GameAudio::play(GameSounds::ENGINE4);

		// Update camera
		float velocity = cars[playerNum]->getVelocity();
		cameras[playerNum]->updateCamera(cars[playerNum]->mGeometry->getModelMatrix(), velocity, cars[playerNum]->isReversing(), isReversing);

		//Update sound - lower start, growth and cap of engine volume with multiplayer
		if (State::numHumanPlayers > 1) {
			if (playerNum == 0) GameAudio::setVolume(GameSounds::ENGINE, 0.1f + 0.0001f * abs(velocity));
			if (playerNum == 1) GameAudio::setVolume(GameSounds::ENGINE2, 0.1f + 0.0001f * abs(velocity));
			if (playerNum == 2) GameAudio::setVolume(GameSounds::ENGINE3, 0.1f + 0.0001f * abs(velocity));
			if (playerNum == 3) GameAudio::setVolume(GameSounds::ENGINE4, 0.1f + 0.0001f * abs(velocity));
		}
		else GameAudio::setVolume(GameSounds::ENGINE,0.1f + 0.0001f * abs(velocity));

		shaderProgram.use();

		// first render to depth map ---------------
		simpleDepthShader.use();

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		doorManager.simulate();

		// First pass
		centerArea.draw(simpleDepthShader, camera, true, 1);
		arenaPlane.draw(simpleDepthShader, camera, true, 1);
		innerWalls.draw(simpleDepthShader, camera, true, 1);
		dome.draw(simpleDepthShader, camera, true, 1);
		walls.draw(simpleDepthShader, camera, true, 1);
		// don't include skybox in depth map

		if (State::redArena) {
			if (doorManager.gateDownOffset < 0) {
				redGates.draw(simpleDepthShader, camera, true, 2, doorManager.gateDownOffset);
				blueGates.draw(simpleDepthShader, camera, true, 2, doorManager.gateUpOffset);
			}
			else redGates.draw(simpleDepthShader, camera, true, 2);
		}
		if (State::blueArena) {
			if (doorManager.gateDownOffset < 0) {
				redGates.draw(simpleDepthShader, camera, true, 2, doorManager.gateUpOffset);
				blueGates.draw(simpleDepthShader, camera, true, 2, doorManager.gateDownOffset);
			}
			else blueGates.draw(simpleDepthShader, camera, true, 2);
		}

		for (const auto& entity : entities)
			entity->draw(physics, simpleDepthShader, camera, true);

		powerUpManager.draw(physics, simpleDepthShader, camera, true);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(x, y, width, height);

		// Now standard rendering -----------------
		shaderProgram.use();

		float near_plane = 100.f, far_plane = 800.f;

		shaderProgram.setFloat("near_plane", near_plane);
		shaderProgram.setFloat("far_plane", far_plane);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glActiveTexture(GL_TEXTURE0);

		// Second pass
		centerArea.draw(shaderProgram, *cameras[playerNum], false, 1);
		arenaPlane.draw(shaderProgram, *cameras[playerNum], false, 1);
		innerWalls.draw(shaderProgram, *cameras[playerNum], false, 1);
		dome.draw(shaderProgram, *cameras[playerNum], false, 1);
		walls.draw(shaderProgram, *cameras[playerNum], false, 1);
		skybox.draw(shaderProgram, *cameras[playerNum], false, 0);
		
		if (State::redArena) {
			if (doorManager.gateDownOffset < 0) {
				redGates.draw(shaderProgram, *cameras[playerNum], false, 2, doorManager.gateDownOffset);
			    blueGates.draw(shaderProgram, *cameras[playerNum], false, 2, doorManager.gateUpOffset);

			} else redGates.draw(shaderProgram, *cameras[playerNum], false, 2);
		}
		if (State::blueArena) {
			if (doorManager.gateDownOffset < 0) {
				redGates.draw(shaderProgram, *cameras[playerNum], false, 2, doorManager.gateUpOffset);
				blueGates.draw(shaderProgram, *cameras[playerNum], false, 2, doorManager.gateDownOffset);
			}
			else blueGates.draw(shaderProgram, *cameras[playerNum], false, 2);
		}
		for (const auto& entity : entities)
			entity->draw(physics, shaderProgram, *cameras[playerNum], false);

		powerUpManager.draw(physics, shaderProgram, *cameras[playerNum], false);

		if (!State::flagPickedUpBy[playerNum])
			gameUI.setCompassDirection(cars[playerNum]->mGeometry->getModelMatrix(), flag.mGeometry->getModelMatrix());
		else if (State::flagPickedUpBy[playerNum])
			gameUI.setCompassDirection(cars[playerNum]->mGeometry->getModelMatrix(), State::flagDropoffBoxes[playerNum]->getGlobalPose().p);

		gameUI.render(playerNum);
	};

	auto mainLoop = [&]()
	{
		processVehicleInput(car);
		if (State::numHumanPlayers >= 2) processVehicleInput(opponentCar1);
		if (State::numHumanPlayers >= 3) processVehicleInput(opponentCar2);
		if (State::numHumanPlayers >= 4) processVehicleInput(opponentCar3);

		powerUpManager.pickup(physics);

		processPowerupInput(car, 0);
		if (State::numHumanPlayers >= 2) processPowerupInput(opponentCar1, 1);
		if (State::numHumanPlayers >= 3) processPowerupInput(opponentCar2, 2);
		if (State::numHumanPlayers >= 4) processPowerupInput(opponentCar3, 3);

		//arena door switch
		if (doorManager.arenaSwitch) {
			if (State::blueArena) {
				updateWorldGridArenaToRedArena();
				physics.generateRedDoor(); //switch from blue doors to red
				State::redArena = true;
				State::blueArena = false;
				//draw red arena
				fmt::print("Doors switching\n");
				fmt::print("Red arena loaded\n");
			}
			else if (State::redArena) {
				updateWorldGridArenaToBlueArena();
				physics.generateBlueDoor(); //switch from red doors to blue
				State::blueArena = true;
				State::redArena = false;
				//draw blue arena
				fmt::print("Doors switching\n");
				fmt::print("Blue arena loaded\n");
			}
			doorManager.arenaSwitch = false;
			doorManager.arenaTimer = 0;
		}

		// TODO: Only compute the AI paths that are actually needed
		if (aiStuffCounter % 3 == 0 && State::numHumanPlayers < 2) { //stagger pathfinding on different frames
			if (State::flagPickedUpBy[1]) {
				opponentBrains[0].updatePath(State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().p, State::flagDropoffBoxes[1]->getGlobalPose().p);
			}
			else {
				opponentBrains[0].updatePath(State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}
		else if (aiStuffCounter % 3 == 1 && State::numHumanPlayers < 3) {
			if (State::flagPickedUpBy[2]) {
				opponentBrains[1].updatePath(State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().p, State::flagDropoffBoxes[2]->getGlobalPose().p);
			}
			else {
				opponentBrains[1].updatePath(State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}
		else if (State::numHumanPlayers < 4) {
			if (State::flagPickedUpBy[3]) {
				opponentBrains[2].updatePath(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p, State::flagDropoffBoxes[3]->getGlobalPose().p);
			}
			else {
				opponentBrains[2].updatePath(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}


		if (State::numHumanPlayers < 2) {
			std::map<MovementFlags, bool>  command = opponentBrains[0].getInput(State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().p, opponentCar1.mGeometry->getModelMatrix().column2.getXYZ());
			opponentCar1.processInput(command);
			powerUpManager.use(physics, command, 1);
		}
		if (State::numHumanPlayers < 3) {
			const std::map<MovementFlags, bool>  command = opponentBrains[1].getInput(State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().p, opponentCar2.mGeometry->getModelMatrix().column2.getXYZ());
			opponentCar2.processInput(command);
			powerUpManager.use(physics, command, 2);
		}
		if (State::numHumanPlayers < 4) {
			const std::map<MovementFlags, bool>  command = opponentBrains[2].getInput(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p, opponentCar3.mGeometry->getModelMatrix().column2.getXYZ());
			opponentCar3.processInput(command);
			powerUpManager.use(physics, command, 3);
		}

		aiStuffCounter++;

		for (const auto& entity : entities)
			entity->simulate(physics);

		powerUpManager.simulate(physics);

		if (State::slowCar.has_value())
		{
			int slowCar = State::slowCar.value();

			PxVehicleEngineData eng;
			eng.mMaxOmega = 800.f;
			eng.mPeakTorque = 2000.f;

			cars[slowCar]->getVehicle()->mDriveSimData.setEngineData(eng);
		} else
		{
			for (int c = 0; c < 4; ++c)
			{
				if (cars[c]->getVehicle()->mDriveSimData.getEngineData().mMaxOmega != 2000.f)
				{
					PxVehicleEngineData eng;
					eng.mMaxOmega = 2000.f;
					eng.mPeakTorque = 2000.f;
					cars[c]->getVehicle()->mDriveSimData.setEngineData(eng);
				}
			}
		}
		
		if (State::killCars[0]) {
			State::flagPickedUpBy[0] = false;
			State::flagPickedUp = false;
			State::canPickupFlag = false;
			State::startPickupFlagTimer = true;
			car.getVehicle()->getRigidDynamicActor()->release();
			car.attachPhysics(physics);
			fmt::print("Respawning player\n");
			State::killCars[0] = false;
			if (State::slowCar == 0)
				State::slowCar.reset();
		}
		if (State::killCars[1]) {
			State::flagPickedUpBy[1] = false;
			State::flagPickedUp = false;
			State::canPickupFlag = false;
			State::startPickupFlagTimer = true;
			opponentCar1.getVehicle()->getRigidDynamicActor()->release();
			opponentCar1.attachPhysics(physics);
			fmt::print("Respawning opponent 1\n");
			State::killCars[1] = false;
			if (State::slowCar == 1)
				State::slowCar.reset();
		}
		if (State::killCars[2]) {
			State::flagPickedUpBy[2] = false;
			State::flagPickedUp = false;
			State::canPickupFlag = false;
			State::startPickupFlagTimer = true;
			opponentCar2.getVehicle()->getRigidDynamicActor()->release();
			opponentCar2.attachPhysics(physics);
			fmt::print("Respawning opponent 2\n");
			State::killCars[2] = false;
			if (State::slowCar == 2)
				State::slowCar.reset();
		}
		if (State::killCars[3]) {
			State::flagPickedUpBy[3] = false;
			State::flagPickedUp = false;
			State::canPickupFlag = false;
			State::startPickupFlagTimer = true;
			opponentCar3.getVehicle()->getRigidDynamicActor()->release();
			opponentCar3.attachPhysics(physics);
			fmt::print("Respawning opponent 3\n");
			State::killCars[3] = false;
			if (State::slowCar == 3)
				State::slowCar.reset();
		}
		physics.stepPhysics();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (State::numHumanPlayers == 1)
			render(0, 0, width, height, 0);

		// If there are more then one human players, draw the four player layout so that there are not
		// weird aspect ratios
		if (State::numHumanPlayers > 1) {
			render(0, height / 2, width / 2, height / 2, 0);
			render(width / 2, height / 2, width / 2, height / 2, 1);
			render(0, 0, width / 2, height / 2, 2);
			render(width / 2, 0, width / 2, height / 2, 3);
		}

		// Check to see if any of the players have won
		if (std::any_of(std::begin(State::scores), std::end(State::scores), [](int score){
				return score >= State::winScore;
			}))
		{
			gameFinished = true;
			gameStarted = false;
		}

#ifdef _DEBUG
		ImGui::Text("Camera Position");
		ImGui::SliderFloat("Camera angle", &angle, -2.0f * M_PI, 2.0f * M_PI);
		ImGui::SliderFloat("x Scale", &g_scale.x, 0.01f, 3.0f);
		ImGui::SliderFloat("y Scale", &g_scale.y, 0.01f, 3.0f);
		ImGui::SliderFloat("Position x", &g_pos.x, -3.0f, 3.0f);
		ImGui::SliderFloat("Position y", &g_pos.y, -3.0f, 3.0f);
		ImGui::SliderFloat("Projectile speed scaling factor", &scalingFactor, 1.0f, 5.0f);

		ImGui::Text("Car Stuff (Press R after changing values)");
		ImGui::Text("VehicleCreate.cpp (lines 38-41)");
		ImGui::SliderFloat("Engine Peak Torque", &peakTorque, 100.f, 2000.f);
		ImGui::SliderFloat("Engine Max Omega (Speed)", &maxOmega, 500.f, 2000.f);
		ImGui::SliderFloat("Gear Switch Time", &gearSwitchTime, 0.f, 50.f);
		ImGui::SliderFloat("Clutch Strength", &clutchStrength, 0.f, 100.f);
		ImGui::Text("Physics.cpp (lines 14-24)");
		ImGui::SliderFloat("Chassis Mass", &chassMass, 1000.f, 2000.f);
		ImGui::SliderFloat("Chassis Dimension X", &chassDimX, 1.f, 5.f);
		ImGui::SliderFloat("Chassis Dimension Y", &chassDimY, 1.f, 4.f);
		ImGui::SliderFloat("Chassis Dimension Z", &chassDimZ, 1.f, 10.f);
		ImGui::SliderFloat("Wheel Mass", &whMass, 20.f, 60.f);
		ImGui::SliderFloat("Wheel Radius", &whRadius, 0.1f, 2.f);
		ImGui::SliderFloat("Wheel Width", &whWidth, 0.1f, 2.f);
		ImGui::SliderFloat("Spring Max Compression", &springMaxCompression, 0.f, 1.f);
		ImGui::SliderFloat("Spring Max Droop", &springMaxDroop, 0.f, 1.f);
		ImGui::SliderFloat("Spring Strength", &springStrength, 0.f, 50000.f);
		ImGui::SliderFloat("Spring Damper Rate", &springDamperRate, 0.f, 10000.f);
		ImGui::Text("Vehicle.cpp (lines 8-25");
		ImGui::SliderFloat("rise rate accel", &riseRateAccel, 0.1f, 500.f);
		ImGui::SliderFloat("rise rate brake", &riseRateBrake, 0.1f, 500.f);
		ImGui::SliderFloat("rise rate handbrake", &riseRateHandBrake, 0.1f, 50.f);
		ImGui::SliderFloat("rise rate steer left", &riseRateSteerLeft, 0.1f, 50.f);
		ImGui::SliderFloat("rise rate steer right", &riseRateSteerRight, 0.1f, 50.f);
		ImGui::SliderFloat("fall rate accel", &fallRateAccel, 0.1f, 500.f);
		ImGui::SliderFloat("fall rate brake", &fallRateBrake, 0.1f, 500.f);
		ImGui::SliderFloat("fall rate handbrake", &fallRateHandBrake, 0.1f, 50.f);
		ImGui::SliderFloat("fall rate steer left", &fallRateSteerLeft, 0.1f, 50.f);
		ImGui::SliderFloat("fall rate steer right", &fallRateSteerRight, 0.1f, 50.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData1A", &gSteerVsForwardSpeedData1A, 0.f, 10.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData1B", &gSteerVsForwardSpeedData1B, 0.f, 10.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData2A", &gSteerVsForwardSpeedData2A, 0.f, 20.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData2B", &gSteerVsForwardSpeedData2B, 0.f, 10.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData3A", &gSteerVsForwardSpeedData3A, 0.f, 100.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData3B", &gSteerVsForwardSpeedData3B, 0.f, 10.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData4A", &gSteerVsForwardSpeedData4A, 0.f, 500.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData4B", &gSteerVsForwardSpeedData4B, 0.f, 10.f); 
#endif
	};
	bool keyboardUsed = false;
	std::vector<int> controllerNumbersUsed;

	// Loop until the user closes the window
	while (!quit)
	{
		capTimer.start();

		quit = input.HandleInput();
		info = input.getInfo(); // Get the keyboard info

		float averageFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
		if (averageFPS > 2000000.f)
			averageFPS = 0;

		// Render
		window.startImGuiFrame();
		Window::clear();

#ifdef _DEBUG
		ImGui::Begin("Framerate Counter!");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
#endif

		if (!gameStarted && !gameFinished && !showControls)
			preLoop();
		if (showControls)
			controlLoop();
		else if (gameStarted && !playersSelected)
		{
			auto controllerInfo = input.getAllControllerInfo();

			// First handle the keyboard inputs
			if (info->inputReleased(MovementFlags::ACTION))
			{
				for (auto* const player : cars)
				{
					if (!player->isHuman && !keyboardUsed)
					{
						player->setHuman(true);
						keyboardUsed = true;
						break;
					}
					if (player->isHuman && player->useKeyboard)
					{
						player->ready = true;
						fmt::print("Keyboard player is ready!\n");
					}
				}
			}

			// Then handle the controller inputs
			for (auto& [id, c_info] : *controllerInfo)
			{
				if (c_info.inputReleased(MovementFlags::ACTION))
				{
					for (auto* const player : cars)
					{
						if (!player->isHuman && std::find(controllerNumbersUsed.begin(), controllerNumbersUsed.end(), id) == controllerNumbersUsed.end())
						{
							player->setHuman(false, id);
							controllerNumbersUsed.push_back(id);
							break;
						}
						if (player->isHuman && player->useController && player->controllerNumber == id)
						{
							player->ready = true;
							fmt::print("Controller player is ready!\n");
						}
					}
				}
			}

			// If at least one player is a human, and all the human players are ready then all players are ready
			int humanCount = 0;
			int readyCount = 0;
			for (auto* const player : cars)
			{
				if (player->isHuman)
				{
					humanCount += 1;
					if (player->ready)
						readyCount += 1;
				}
			}

			if (humanCount >= 1 && readyCount == humanCount)
			{
				playersSelected = true;
				State::numHumanPlayers = humanCount;
			}

			// Render all four players
			render(0, height / 2, width / 2, height / 2, 0, true);
			gameUI.renderPlayerSelect(car.isHuman, car.ready);

			render(width / 2, height / 2, width / 2, height / 2, 1, true);
			gameUI.renderPlayerSelect(opponentCar1.isHuman, opponentCar1.ready);

			render(0, 0, width / 2, height / 2, 2, true);
			gameUI.renderPlayerSelect(opponentCar2.isHuman, opponentCar2.ready);

			render(width / 2, 0, width / 2, height / 2, 3, true);
			gameUI.renderPlayerSelect(opponentCar3.isHuman, opponentCar3.ready);

		}
		else if (gameStarted && !gameFinished)
			mainLoop();
		else if (gameFinished)
		{
			if (State::numHumanPlayers == 1)
			{
				render(0, 0, width, height, 0, true);
				gameUI.renderEndScreen(0);
			}

			if (State::numHumanPlayers > 1)
			{
				render(0, height / 2, width / 2, height / 2, 0, true);
				gameUI.renderEndScreen(0);

				render(width / 2, height / 2, width / 2, height / 2, 1, true);
				gameUI.renderEndScreen(1);

				render(0, 0, width / 2, height / 2, 2, true);
				gameUI.renderEndScreen(2);

				render(width / 2, 0, width / 2, height / 2, 3, true);
				gameUI.renderEndScreen(3);
			}

			// If any of the players press enter, reset the game
			if (!info->inputState[MovementFlags::ENTER])
			{
				for (auto* c : cameras)
				{
					c->lagCounter = 0;
				}
				State::scores.fill(0);
				State::flagPickedUp = false;
				State::killCars.fill(true);
				State::flagPickedUpBy.fill(false);

				gameFinished = false;
				gameStarted = false;
				playersSelected = true; // No need to reset who is playing if they want to play again

				if (State::blueArena) {
					updateWorldGridArenaToRedArena();
					physics.generateRedDoor(); //switch from blue doors to red
					State::redArena = true;
					State::blueArena = false;
					//draw red arena
					fmt::print("Red arena loaded\n");
				}
			}
		}


#ifdef _DEBUG
		ImGui::End();
		Window::renderImGuiFrame();
#endif
		window.swap();

		countedFrames += 1;

		int frameTicks = capTimer.getTicks();
		if (frameTicks < SCREEN_TICKS_PER_FRAME)
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
	}

	//cleanup
	for (const auto& entity : entities)
		entity->cleanUpPhysics();
	powerUpManager.cleanUp();
	physics.CleanupPhysics();
	GameAudio::cleanup();

	return 0;
}
