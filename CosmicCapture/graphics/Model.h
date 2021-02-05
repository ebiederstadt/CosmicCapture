#pragma once

#include <vector>
#include <memory>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Mesh.h"
#include "ShaderProgram.h"
#include "GraphicsCamera.h"


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
		std::shared_ptr<GraphicsCamera> camera
	);

	void draw();

	// Movement and position updates
	// Add helper functions here if you need them
	void move(const glm::vec3& amount) { modelMatrix = translate(modelMatrix, amount); }

	void scale(const float amount) { modelMatrix = glm::scale(modelMatrix, { amount, amount, amount }); }
	void scale(const glm::vec3& amount) { modelMatrix = glm::scale(modelMatrix, amount); }

	void rotateX(const float angle) { modelMatrix = rotate(modelMatrix, angle, { 1.0f, 0.0f, 0.0f }); }
	void rotateY(const float angle) { modelMatrix = rotate(modelMatrix, angle, { 0.0f, 1.0f, 0.0f }); }
	void rotateZ(const float angle) { modelMatrix = rotate(modelMatrix, angle, { 0.0f, 0.0f, 1.0f }); }

	// Note: Try to avoid manipulating the model matrix directly,
	// Add helper functions if you can
	glm::mat4 modelMatrix;

private:
	std::vector<Mesh> mMeshes; // Each model is made of one or more meshes 
	Texture mTexture; // Assuming that each texture is unique to each model (may need to rework)
	std::shared_ptr<ShaderProgram> mShaderPointer; // Shaders can be shared between multiple models
	std::shared_ptr<GraphicsCamera> mCameraPointer; // Camera object is shared among all the meshes

	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh);

	void viewPipeLine();
};

