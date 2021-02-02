#include <string>
#include <memory>
#include <map>
#include <fmt/format.h>
#include <GL/glew.h>
#include <SDL/SDL.h>

#include "graphics/Window.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Model.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"

//PHYSX Stuff----------------------------------------------------------
#include <ctype.h>

#include "physx/PxPhysicsAPI.h"
#include "physx/vehicle/PxVehicleSDK.h"
#define PX_RELEASE(x) if(x){x->release();x=NULL;}

using namespace physx;

PxDefaultErrorCallback gErrorCallback;
PxDefaultAllocator gAllocator;
PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;

PxCooking* gCooking = NULL;

PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

//VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
PxBatchQuery* gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxRigidStatic* gGroundPlane = NULL;
PxVehicleDrive4W* gVehicle4W = NULL;

bool					gIsVehicleInAir = true;


void initPhysics() {
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	bool recordMemoryAllocations = true;
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), recordMemoryAllocations, gPvd);
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	PxU32 numWorkers = 1;
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;

	gScene = gPhysics->createScene(sceneDesc);

	fmt::print("Physx initialized\n");
}

void cleanupPhysics()
{	
	PX_RELEASE(gMaterial);
	PX_RELEASE(gCooking);
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	fmt::print("Physx cleaned up\n");
} 
//---------------------------------------------------------------------


void renderGeometry(const Window& window, std::vector<Model>& geometry)
{
	window.startImGuiFrame();
	Window::clear();

	for (auto& model : geometry)
		model.draw();

	ImGui::Begin("Framerate Counter!");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	window.swap();
}


int main(int argc, char** args) {
	// Window Initialization
	const GLint width = 1280, height = 720;
	Window window("Cosmic Capture", width, height);

	// Physics initialization
	initPhysics();

	// Shaders are used 1+ times, and shared between all models that use them
	const auto shaderProgram = std::make_shared<ShaderProgram>("shaders/main.vert", "shaders/main.frag");

	// The camera is used once, and shared between all geometry
	const auto camera = std::make_shared<Camera>();

	std::vector<Model> models;
	models.emplace_back("models/monkey.ply", "textures/camouflage.jpg", shaderProgram, camera);
	models[0].scale(0.5f);

	glm::vec3 movementVector(0.0f);

	// Loop until the user closes the window
	bool shouldClose = false;
	const char* name;
	
	while (!shouldClose) {
		if (SDL_PollEvent(&window.event)) {
			switch (window.event.type)
			{
			case SDL_QUIT:
				shouldClose = true;
				break;

			case SDL_KEYDOWN:
				switch (window.event.key.keysym.sym)
				{
				case SDLK_LEFT:
					movementVector.x += -0.03f;
					break;

				case SDLK_RIGHT:
					movementVector.x += 0.03f;
					break;

				case SDLK_UP:
					movementVector.y += 0.03f;
					break;

				case SDLK_DOWN:
					movementVector.y += -0.03f;
					break;

				case SDLK_MINUS:
					movementVector = glm::vec3(0.0f);
				}
			}
		}

		models[0].move(movementVector);
	 	renderGeometry(window, models);
	}


	cleanupPhysics();

	return 0;
}

