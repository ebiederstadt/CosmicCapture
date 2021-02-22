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
	Model arena("models/basic_arena.ply", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);

	auto wheel1 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto wheel2 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto wheel3 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto wheel4 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto wheel5 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	auto wheel6 = std::make_shared<Model>("models/cube.ply", "textures/wall.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);

	auto body = std::make_unique<Model>("models/cube.ply", "textures/camouflage.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
  
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
  models.push_back(dynamicBall);
  models.push_back(staticWall);
  models.push_back(flag);
  models.push_back(dropoffZone);

	//main loop flag
	bool quit = false;

	Vehicle car(std::move(body));
	car.attachPhysics(physics);

	// Loop until the user closes the window
	while (!quit) {

		quit = input.HandleInput();

		// Physics simulation
		auto inputState = input.getInputState();

		// Repeat for all vehicles eventually...
		car.processInput(inputState);
		car.simulate(physics);
		
		physics.stepPhysics();

		// Render
		window.startImGuiFrame();
		Window::clear();

		// Update camera
		sCamera->updateCamera(car.mGeometry->getModelMatrix());

		shaderProgram.use();

		// Draw arena
		arena.drawArena();

		car.draw(physics);

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
