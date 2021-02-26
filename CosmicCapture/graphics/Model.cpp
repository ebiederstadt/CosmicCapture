#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <fmt/format.h>
#include <glm/gtc/type_ptr.hpp>


Model::Model(
	const char* modelPath,
	const char* texturePath,
	const ShaderProgram& shaderProgram,
	std::shared_ptr<Camera> camera,
	const unsigned int usage
) :
	mTexture(texturePath, GL_LINEAR),
	mShaderID(static_cast<unsigned int>(shaderProgram)),
	mCameraPointer(std::move(camera)),
	mUsage(usage),
	mModel(physx::PxIdentity)

{
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

/*

void Model::draw(const physx::PxMat44& modelMatrix)
{

	setModel(modelMatrix);
	
	mTexture.bind();

	// View pipeline
	const auto modelLoc = glGetUniformLocation(mShaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix.column0.x);

	const auto viewLoc = glGetUniformLocation(mShaderID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mCameraPointer->getViewMatrix()));

	const auto projectionLoc = glGetUniformLocation(mShaderID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mCameraPointer->perspectiveMatrix));

	// Added -> car model not affected by arena lighting yet
	bool lit = false;
	const auto litLoc = glGetUniformLocation(mShaderID, "lit");
	glUniform1i(litLoc, lit);

	for (const auto& mesh : mMeshes)
		mesh.draw();

	Texture::unbind();
}

*/

void Model::draw(const physx::PxMat44& modelMatrix, const ShaderProgram& shaderProgram, bool depth, const unsigned int& depthMap, float ortho)
{

	setModel(modelMatrix);

	mShaderID = static_cast<unsigned int>(shaderProgram);

	if (!depth) {
		glActiveTexture(GL_TEXTURE0);
		mTexture.bind();
	}

	/*
	if (!depth) {

		//glActiveTexture(GL_TEXTURE0);
		mTexture.bind();
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, depthMap);
	}
	 else {
		// const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		// glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		//glBindFramebuffer(GL_FRAMEBUFFER, depthMap);
		//glClear(GL_DEPTH_BUFFER_BIT);
		//glActiveTexture(GL_TEXTURE0);
		mTexture.bind();
		//glBindTexture(GL_TEXTURE_2D, texturePath);
	}
	*/

	// Placing them at opposite ends of x-axis
	glm::vec3 purpleLight = glm::vec3(-300.0f, 300.0f, 0.0f);
	glm::vec3 orangeLight = glm::vec3(300.0f, 300.0f, 0.0f);

	if (!depth) {

		// View pipeline

		const auto purpleLightLoc = glGetUniformLocation(mShaderID, "purpleLight");
		const auto orangeLightLoc = glGetUniformLocation(mShaderID, "orangeLight");

		glUniform3fv(purpleLightLoc, 1, glm::value_ptr(purpleLight));
		glUniform3fv(orangeLightLoc, 1, glm::value_ptr(orangeLight));

		bool lit = true;
		const auto litLoc = glGetUniformLocation(mShaderID, "lit");
		glUniform1i(litLoc, lit);

		const auto viewLoc = glGetUniformLocation(mShaderID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mCameraPointer->getViewMatrix()));

		const auto projectionLoc = glGetUniformLocation(mShaderID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mCameraPointer->perspectiveMatrix));

	}

	auto eye = mCameraPointer->getEye();

	float near_plane = 200.f, far_plane = 600.f;
	//glm::mat4 lightProjection = glm::ortho(eye.y - 100.f, eye.y + 100.f, eye.x - 100.f, eye.x + 100.f, eye.z - 100.f, eye.z + 100.f);
	glm::mat4 lightProjection = glm::ortho(-250.f, 250.f, -250.f, 500.f, 200.f, 600.f);
	//glm::mat4 lightProjection = mCameraPointer->perspectiveMatrix;
	//glm::mat4 lightProjection = glm::ortho(0.f, 200.f, 0.f, 200.f, 0.f, 50.f);

	glm::mat4 lightView = glm::lookAt(orangeLight,
		glm::vec3(-300.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	auto lightSpaceLoc = glGetUniformLocation(mShaderID, "lightSpaceMatrix");
	glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	const auto modelLoc = glGetUniformLocation(mShaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix.column0.x);

	for (const auto& mesh : mMeshes)
		mesh.draw();

	if(!depth) Texture::unbind();
}

void Model::drawArena(const ShaderProgram& shaderProgram, bool depth, const unsigned int & depthMap, float ortho)
{

	mShaderID = static_cast<unsigned int>(shaderProgram);

	if (!depth) {
		glActiveTexture(GL_TEXTURE0);
		mTexture.bind();
	}


	/*
	if (!depth) {

		//glActiveTexture(GL_TEXTURE0);
		mTexture.bind();
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, depthMap);
	}
	 else {
		// const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		// glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		//glBindFramebuffer(GL_FRAMEBUFFER, depthMap);
		//glClear(GL_DEPTH_BUFFER_BIT);
		//glActiveTexture(GL_TEXTURE0);
		mTexture.bind();
		//glBindTexture(GL_TEXTURE_2D, texturePath); 
	}
	*/

	// Placing them at opposite ends of x-axis
	glm::vec3 purpleLight = glm::vec3(-300.0f, 300.0f, 0.0f);
	glm::vec3 orangeLight = glm::vec3(300.0f, 300.0f, 0.0f);

	if (!depth) {

		// View pipeline

		const auto purpleLightLoc = glGetUniformLocation(mShaderID, "purpleLight");
		const auto orangeLightLoc = glGetUniformLocation(mShaderID, "orangeLight");

		glUniform3fv(purpleLightLoc, 1, glm::value_ptr(purpleLight));
		glUniform3fv(orangeLightLoc, 1, glm::value_ptr(orangeLight));

		bool lit = true;
		const auto litLoc = glGetUniformLocation(mShaderID, "lit");
		glUniform1i(litLoc, lit);

		const auto viewLoc = glGetUniformLocation(mShaderID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mCameraPointer->getViewMatrix()));

		const auto projectionLoc = glGetUniformLocation(mShaderID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mCameraPointer->perspectiveMatrix));

	}

	float near_plane = 200.f, far_plane = 600.f;

	auto eye = mCameraPointer->getEye();
	auto dir = mCameraPointer->getDir();

	//glm::mat4 lightProjection = glm::ortho(-ortho, ortho, -ortho, ortho, near_plane, far_plane);
	//glm::mat4 lightProjection = glm::ortho(eye.y - 100.f, eye.y + 100.f, eye.x - 100.f, eye.x + 100.f, -eye.z, -eye.z + 100.f);
	glm::mat4 lightProjection = glm::ortho(-250.f, 250.f, -250.f, 500.f, 200.f, 600.f);
	//glm::mat4 lightProjection = mCameraPointer->perspectiveMatrix;

	glm::mat4 lightView = glm::lookAt(orangeLight,
		glm::vec3(-300.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	auto lightSpaceLoc = glGetUniformLocation(mShaderID, "lightSpaceMatrix");
	glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	const auto modelLoc = glGetUniformLocation(mShaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));


	for (const auto& mesh : mMeshes)
		mesh.draw();

	if(!depth) Texture::unbind();
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
	mMeshes.emplace_back(geometry, mTexture, mUsage);
}
