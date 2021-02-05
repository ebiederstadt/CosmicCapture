#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <fmt/format.h>
#include <glm/gtc/type_ptr.hpp>


Model::Model(
	const char* modelPath,
	const char* texturePath,
	std::shared_ptr<ShaderProgram> shaderProgram,
	std::shared_ptr<GraphicsCamera> camera
) :
	modelMatrix(1.0f),
	mTexture(texturePath, GL_LINEAR),
	mShaderPointer(std::move(shaderProgram)),
	mCameraPointer(std::move(camera))
{
	mShaderPointer->compile();
	mTexture.bind();
	
	Assimp::Importer importer;

	// TODO: Consider the flags set here. Potential for higher quality or higher performance
	const auto* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		fmt::print("ERROR::ASSIMP::{}\n", importer.GetErrorString());
		throw std::runtime_error("Failed to load model");
	}

	processNode(scene->mRootNode, scene);
}

void Model::draw()
{
	// Note: potential performance issue. Don't need to call use for every mesh
	mShaderPointer->use();
	viewPipeLine();

	for (const auto& mesh : mMeshes)
		mesh.draw();
}


void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all of the the meshes associated with the node
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		auto* const mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh);
	}

	// Repeat for all children
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene);
	}
}


void Model::processMesh(aiMesh* mesh)
{
	CpuGeometry geometry;
	
	// Process all of the vertices, normals, and texture coordinates
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		auto vertex = mesh->mVertices[i];
		geometry.vertices.emplace_back(vertex.x, vertex.y, vertex.z);

		auto normal = mesh->mNormals[i];
		geometry.normals.emplace_back(normal.x, normal.y, normal.z);

		if (mesh->mTextureCoords[0])
		{
			auto texCoord = mesh->mTextureCoords[0][i];
			geometry.texCoords.emplace_back(texCoord.x, texCoord.y);
		}
		else
			geometry.texCoords.emplace_back(glm::vec2(1.0f));
	}

	// Process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		const auto face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			geometry.indices.emplace_back(face.mIndices[j]);
	}

	auto numVertices = geometry.vertices.size();
	auto numNormals = geometry.normals.size();
	auto numTextureCoords = geometry.texCoords.size();

	if (numVertices == numNormals && numNormals == numTextureCoords)
	{
		auto highestIndex = *std::max_element(std::begin(geometry.indices), std::end(geometry.indices));
		if (highestIndex >= numVertices)
		{
			fmt::print("Invalid Index: {}", highestIndex);
			throw std::runtime_error("Failed to load model");
		}
	}
	else
	{
		fmt::print("Unequal number of vertices, normals, and texture coordinates: {}, {}, {}", numVertices, numNormals, numTextureCoords);
		throw std::runtime_error("Failed to load model");
	}

	// TODO: material stuff :)

	// Move into a vector, without copying it
	mMeshes.emplace_back(geometry, mTexture);
}


// Note: It might make more sense to have this method be in a different class
void Model::viewPipeLine()
{
	const auto program = static_cast<unsigned int>(*mShaderPointer);
	const auto modelLoc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	const auto viewLoc = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mCameraPointer->viewMatrix));

	const auto projectionLoc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mCameraPointer->projectionMatrix));
}

