// Note: This class is inspired, but not copied from the fall 2020 CPSC 453 OpenGL Boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <physx/PxPhysicsAPI.h>

#include "Shader.h"


class ShaderProgram
{
public:
	ShaderProgram(const char*, const char*);
	~ShaderProgram() { glDeleteProgram(mShaderProgram); }

	explicit operator unsigned int() const { return mShaderProgram; }

	void compile();
	void use() const { glUseProgram(mShaderProgram); }

	void setInt(std::string_view, int) const;
	void setFloat(std::string_view, float) const;
	void setMat4(std::string_view, const glm::mat4&) const;
	void setMat4(std::string_view, const physx::PxMat44&) const;
	void setVec3(std::string_view, const glm::vec3&) const;

private:
	unsigned int mShaderProgram;
	
	Shader mVertex;
	Shader mFragment;

};

