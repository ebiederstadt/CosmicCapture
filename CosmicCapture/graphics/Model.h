#pragma once

#include <vector>
#include <memory>
#include <assimp/scene.h>
#include <physx/PxPhysicsAPI.h>

#include "Mesh.h"
#include "ShaderProgram.h"
#include "TextureAPI.h"
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
	/// <param name="camera">Camera object to use with the model</param>
	/// <param name="usage">openGL Usage type. Default is GL_STATIC_DRAW</param>
	Model(
		const char* modelPath,
		const char* texturePath,
		std::shared_ptr<Camera> camera,
		unsigned int usage = GL_STATIC_DRAW,
		bool isRepeating = false
	);
	Model(const char* modelPath, const glm::vec4& textureColor, std::shared_ptr<Camera> camera);

	// Draw things where the position/rotation/scale changes
	void draw(const physx::PxMat44& modelMatrix, const ShaderProgram& shaderProgram, bool depth, int type);

	// Draw things that stay in the same place
	void draw(const ShaderProgram& shaderProgram, bool depth, int type);

	[[nodiscard]] physx::PxMat44 getModelMatrix() const { return mModel; }

private:
	std::vector<Mesh> mMeshes; // Each model is made of one or more meshes 
	std::shared_ptr<Camera> mCameraPointer; // Camera object is shared among all the meshes

	TextureAPI* api;
	TextureAPI::TextureName mTexture; // Assuming that each texture is unique to each model (may need to rework)

	unsigned int mUsage;

	physx::PxMat44 mModel;
	void setModel(const physx::PxMat44& m) { mModel = m; }

	void readMesh(const char* modelPath);

	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh);
};

