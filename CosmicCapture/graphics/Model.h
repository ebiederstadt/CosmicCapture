#pragma once

#include <vector>
#include <memory>
#include <assimp/scene.h>
#include <physx/PxPhysicsAPI.h>

#include "Mesh.h"
#include "ShaderProgram.h"
#include "../Camera.h"

/// <summary>
/// This class provides a high level interface for interacting with the graphics API.
/// </summary>
class Model
{
public:
	/// <summary>
	/// Create a model object
	/// </summary>
	/// <param name="modelPath">Path to a file representing the 3D model</param>
	/// <param name="texturePath">Path to the image texture to use with the model</param>
	/// <param name="shaderProgram">Vertex + fragment shader to use with the model</param>
	/// <param name="camera">Camera object to use with the model</param>
	/// <param name="usage">openGL Usage type. Default is GL_STATIC_DRAW</param>
	Model(
		const char* modelPath,
		const char* texturePath,
		const ShaderProgram& shaderProgram,
		std::shared_ptr<Camera> camera,
		unsigned int usage = GL_STATIC_DRAW
	);

	void draw(const physx::PxMat44& modelMatrix, const ShaderProgram& shaderProgram, bool depth, const unsigned int& depthMap);
	void drawArena(const ShaderProgram& shaderProgram, bool depth, const unsigned int& depthMap);

	[[nodiscard]] physx::PxMat44 getModelMatrix() const { return mModel; }

private:
	std::vector<Mesh> mMeshes; // Each model is made of one or more meshes 
	Texture mTexture; // Assuming that each texture is unique to each model (may need to rework)
	unsigned int mShaderID; // Shaders can be shared between multiple models
	std::shared_ptr<Camera> mCameraPointer; // Camera object is shared among all the meshes

	unsigned int mUsage;

	physx::PxMat44 mModel;
	void setModel(const physx::PxMat44& m) { mModel = m; }

	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh);
};

