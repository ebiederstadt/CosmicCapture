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

#include "./audio/AudioEngine.h"

#include "Physics.h"
#include "Camera.h"
#include "Render.h"


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

	// Enable depth testing and set up related shader for shhadows
	glEnable(GL_DEPTH_TEST);

	ShaderProgram simpleDepthShader("shaders/simple.vert", "shaders/simple.frag");
	simpleDepthShader.compile();

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
	auto dynamicBall = std::make_shared<Model>("models/ball.ply", "textures/blue.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto staticWall = std::make_shared<Model>("models/static_wall.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto flag = std::make_shared<Model>("models/flag.ply", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto dropoffZone = std::make_shared<Model>("models/dropoff_zone.ply", "textures/dropflaghere.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	//---------------------------------------------

	std::vector<std::shared_ptr<Model>> models;
	models.reserve(11); // Make space for 10 models without the need for copying
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

  // Shadow setup end ---------------------------------------------------------------------

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

		// Update camera
		sCamera->updateCamera(body->getModelMatrix());

		auto counter = 1;

		// first render to depth map ---------------

		simpleDepthShader.use();

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Draw arena
		arena.drawArena(simpleDepthShader, true, depthMapFBO);

		for (auto& model : models)
		{
			model->draw(modelMatrices[counter], simpleDepthShader, true, depthMapFBO);
			++counter;
		}

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

		// Draw arena
		arena.drawArena(shaderProgram, false, depthMap);

		counter = 1;
		for (auto& model : models)
		{
			model->draw(modelMatrices[counter], shaderProgram, false, depthMap);
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
	soundSystem.killSources();

	return 0;
}
