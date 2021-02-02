#pragma once

#include <string>
#include <vector>
#include <memory>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "ShaderProgram.h"
#include "../Camera.h"


/// <summary>
/// This class provides a high level interface for interacting with the graphics API.
/// </summary>
class Model
{
public:
	Model(
		const char* modelPath,
		const char* texturePath,
		std::shared_ptr<ShaderProgram> shaderProgram,
		std::shared_ptr<Camera> camera
	);

	void draw();

	glm::mat4 modelMatrix;

private:
	std::vector<Mesh> mMeshes; // Each mesh is made of one or more meshes
	Texture mTexture; // Assuming that each texture is unique to each model (may need to rework)
	std::shared_ptr<ShaderProgram> mShaderPointer; // Shaders can be shared between multiple models
	std::shared_ptr<Camera> mCameraPointer; // Camera object is shared among all the meshes

	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh);

	void viewPipeLine();
};

