#include <string>
#include <fmt/format.h>
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <glm/vec3.hpp>

#include "graphics/Window.h"
#include "graphics/Geometry.h"
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


// TODO: Shaders should be stored in files :)
const std::string vertexSource = R"glsl(
	#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aCol;

	out vec3 color;

	void main()
	{
		color = aCol;
		gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	}
)glsl";

const std::string fragmentSource = R"glsl(
	#version 330 core
	in vec3 color;

	out vec4 fragColor;

	void main()
	{
		fragColor = vec4(color, 1.0f);	
	}
)glsl";


int main(int argc, char** args) {
	// TODO: Make the window resizable
	const GLint width = 1280, height = 720;

	Window window("Cosmic Capture", width, height);

	// Vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* cVertexSource = vertexSource.c_str();
	glShaderSource(vertexShader, 1, &cVertexSource, nullptr);
	glCompileShader(vertexShader);

	// Fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* cFragmentSource = fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &cFragmentSource, nullptr);
	glCompileShader(fragmentShader);

	// Link Shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		fmt::print("ERROR::SHADER::PROGRAM::LINKING_FAILED\n {}", infoLog);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	CpuGeometry cpuGeom;
	cpuGeom.vertices = {
		glm::vec3(0.5f,  0.5f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f),
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(-0.5f,  0.5f, 0.0f)
	};
	cpuGeom.cols = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 1.0f),
	};
	cpuGeom.indices = {
		0, 1, 3,
		1, 2, 3
	};

	GpuGeometry gpuGeometry;
	gpuGeometry.uploadData(cpuGeom);

	initPhysics();

	// Loop until the user closes the window
	while (true) {
		if (SDL_PollEvent(&window.event)) {
			if (window.event.type == SDL_QUIT) break;
		}
		window.startImGuiFrame();
		Window::clear();

		gpuGeometry.drawData();

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Framerate Counter!");			
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		window.swap();
	}

	cleanupPhysics();

	return 0;
}


