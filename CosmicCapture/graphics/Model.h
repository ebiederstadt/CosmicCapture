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
	Model(
		const char* modelPath,
		const char* texturePath
	);

	// Draw things where the position/rotation/scale changes
	void draw(const physx::PxMat44&, const ShaderProgram&, const Camera&, bool, int);

	// Draw things that stay in the same place
	void draw(const ShaderProgram&, const Camera&, bool, int, int offset = 0);

	[[nodiscard]] physx::PxMat44 getModelMatrix() const { return mModel; }

private:
	std::vector<Mesh> mMeshes; // Each model is made of one or more meshes 

	TextureAPI* api;
	TextureAPI::TextureName mTexture; // Assuming that each texture is unique to each model (may need to rework)

	physx::PxMat44 mModel;
	void setModel(const physx::PxMat44& m) { mModel = m; }

	void readMesh(const char*);

	void processNode(aiNode*, const aiScene*);
	void processMesh(aiMesh*);
};

