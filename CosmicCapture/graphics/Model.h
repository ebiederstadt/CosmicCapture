#pragma once

#include <string>
#include <vector>
#include <assimp/scene.h>

#include "Mesh.h"


/// <summary>
/// This class provides a high level interface for interacting with the graphics API.
/// </summary>
class Model
{
public:
	Model(const std::string& modelPath, const std::string& texturePath);

	void draw();

private:
	std::vector<Mesh> mMeshes;
	Texture mTexture;

	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh);
};

