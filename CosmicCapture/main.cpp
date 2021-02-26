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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Physics.h"
#include "Camera.h"
#include "Render.h"
#include <stb/stb_image.h>

#include <glm/glm.hpp>

#include <iostream>

#define M_PI  3.14159265358979323846

float angle = -0.25f;

void renderQuad();
void renderScene(const ShaderProgram& shader);
void renderCube();
unsigned int loadTexture(const char* path);

unsigned int planeVAO;

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

	glEnable(GL_DEPTH_TEST);

	ShaderProgram shaderProgram("shaders/main.vert", "shaders/main.frag");
	shaderProgram.compile();

	ShaderProgram simpleDepthShader("shaders/simple.vert", "shaders/simple.frag");
	simpleDepthShader.compile();

	ShaderProgram debugDepth("shaders/debugDepth.vert", "shaders/debugDepth.frag");
	debugDepth.compile();

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

  // Shadow setup start -------------------------------------

	  // set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

  float plane = 600.f;

  float pHeight = -1.f;

  float planeVertices[] = {
	  // positions            // normals         // texcoords
	   plane, pHeight,  plane,  0.0f, 1.0f, 0.0f,  plane,  0.0f,
	  -plane, pHeight,  plane,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
	  -plane, pHeight, -plane,  0.0f, 1.0f, 0.0f,   0.0f, plane,

	   plane, pHeight,  plane,  0.0f, 1.0f, 0.0f,  plane,  0.0f,
	  -plane, pHeight, -plane,  0.0f, 1.0f, 0.0f,   0.0f, plane,
	   plane, pHeight, -plane,  0.0f, 1.0f, 0.0f,  plane, 10.0f
  };

  
  // plane VAO
  unsigned int planeVBO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glBindVertexArray(0);


  unsigned int woodTexture = loadTexture("textures / wall.jpg");

  // Configure depth map FBO
  unsigned int depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  const unsigned int SHADOW_WIDTH = 1024*4, SHADOW_HEIGHT = 1024*4;

  // create depth texture
  unsigned int depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	  SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

  shaderProgram.use();

  auto mShaderID = static_cast<unsigned int>(shaderProgram);
  auto samplerLoc = glGetUniformLocation(mShaderID, "textureSampler");
  glUniform1i(samplerLoc, 0);
  samplerLoc = glGetUniformLocation(mShaderID, "shadowMap");
  glUniform1i(samplerLoc, 1);

  debugDepth.use();

  mShaderID = static_cast<unsigned int>(debugDepth);
  samplerLoc = glGetUniformLocation(mShaderID, "depthMap");
  glUniform1i(samplerLoc, 0);


  // Shadow setup end ---------------------------------------------------------------------


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

		//glm::vec3 purpleLight = glm::vec3(-300.0f, 300.0f, 0.0f);
		//glm::vec3 orangeLight = glm::vec3(100.0f, 100.0f, 0.0f);

		float ortho = 200.f;  // not really relevant anymore


		float near_plane = 200.f, far_plane = 600.f;
		//glm::mat4 lightProjection = glm::ortho(-ortho, ortho, -ortho, ortho, near_plane, far_plane);

		//float near_plane = 1.0f, far_plane = 250.f;
		//glm::mat4 lightProjection = glm::ortho(-300.0f, 300.0f, -300.0f, 300.0f, near_plane, far_plane);

		//glm::mat4 lightView = glm::lookAt(orangeLight,
			//glm::vec3(-100.0f, 0.0f, 0.0f),
			//glm::vec3(0.0f, 1.0f, 0.0f));

		//glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		simpleDepthShader.use();

		//mShaderID = static_cast<unsigned int>(simpleDepthShader);
		//auto lightSpaceLoc = glGetUniformLocation(mShaderID, "lightSpaceMatrix");
		//glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

		// glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		// glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		// glClear(GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, woodTexture);
			//glBindVertexArray(planeVAO);
			//glDrawArrays(GL_TRIANGLES, 0, 6);
			//renderScene(simpleDepthShader);

		
		// Draw arena
		arena.drawArena(simpleDepthShader, true, depthMapFBO, ortho);

		for (auto& model : models)
		{
			model->draw(modelMatrices[counter], simpleDepthShader, true, depthMapFBO, ortho);
			++counter;
		}

		

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		//glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Now standard rendering -----------------


		shaderProgram.use();

		// glActiveTexture(GL_TEXTURE1);
		// glBindTexture(GL_TEXTURE_2D, depthMap);

		//mShaderID = static_cast<unsigned int>(shaderProgram);
		//lightSpaceLoc = glGetUniformLocation(mShaderID, "lightSpaceMatrix");
		//glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

		auto nearLoc = glGetUniformLocation(mShaderID, "near_plane");
		glUniform1f(nearLoc, near_plane);

		auto farLoc = glGetUniformLocation(mShaderID, "far_plane");
		glUniform1f(farLoc, far_plane);

		/*
		arena.drawArena(shaderProgram, false, depthMap);

		counter = 1;
		for (auto& model : models)
		{
			model->draw(modelMatrices[counter], shaderProgram, false, depthMap);
			++counter;
		}
		*/

		

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		//glBindVertexArray(planeVAO);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glActiveTexture(GL_TEXTURE0);
		
		//renderScene(shaderProgram);

		// Draw arena
		arena.drawArena(shaderProgram, false, depthMap, ortho);

		counter = 1;
		for (auto& model : models)
		{
			model->draw(modelMatrices[counter], shaderProgram, false, depthMap, ortho);
			++counter;
		}


		
		/*
		debugDepth.use();

		glBindVertexArray(planeVAO);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		mShaderID = static_cast<unsigned int>(debugDepth);

		nearLoc = glGetUniformLocation(mShaderID, "near_plane");
		glUniform1f(nearLoc, near_plane);

		farLoc = glGetUniformLocation(mShaderID, "far_plane");
		glUniform1f(farLoc, far_plane);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		renderQuad();
		*/


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

	// optional: de-allocate all resources once they've outlived their purpose:
// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);

	return 0;
}


// renders the 3D scene
// --------------------
void renderScene(const ShaderProgram& shader)
{

	auto mShaderID = static_cast<unsigned int>(shader);

	// floor
	glm::mat4 model = glm::mat4(1.0f);

	auto modelLoc = glGetUniformLocation(mShaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//glBindVertexArray(planeVAO);

	// cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));

	modelLoc = glGetUniformLocation(mShaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	renderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));

	modelLoc = glGetUniformLocation(mShaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	renderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));

	modelLoc = glGetUniformLocation(mShaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	renderCube();
}


// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
