#include "ShaderProgram.h"

#include <stdexcept>
#include <fmt/format.h>
#include <glm/gtc/type_ptr.hpp>


ShaderProgram::ShaderProgram(const char* vertLoc, const char* fragLoc)
{
	mVertex = Shader(vertLoc, shaderTypes::VERTEX);
	mFragment = Shader(fragLoc, shaderTypes::FRAGMENT);

	mShaderProgram = glCreateProgram();
}


void ShaderProgram::compile()
{
	mVertex.compile();
	mFragment.compile();

	glAttachShader(mShaderProgram, mVertex.getID());
	glAttachShader(mShaderProgram, mFragment.getID());
	glLinkProgram(mShaderProgram);

#ifdef _DEBUG
	int success;
	char infoLog[512];
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(mShaderProgram, 512, nullptr, infoLog);
		fmt::print("ERROR::SHADER::PROGRAM::LINKING_FAILED\n {}", infoLog);
		throw std::runtime_error("Failed: Shader linking");
	}
#endif // DEBUG

	mVertex.cleanUp();
	mFragment.cleanUp();
}

void ShaderProgram::setInt(std::string_view name, int value) const
{
	const auto location = glGetUniformLocation(mShaderProgram, name.data());
	glUniform1i(location, value);
}

void ShaderProgram::setFloat(std::string_view name, float value) const
{
	const auto location = glGetUniformLocation(mShaderProgram, name.data());
	glUniform1f(location, value);
}

void ShaderProgram::setMat4(std::string_view name, const glm::mat4& value) const
{
	const auto location = glGetUniformLocation(mShaderProgram, name.data());
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::setMat4(std::string_view name, const physx::PxMat44& value) const
{
	const auto location = glGetUniformLocation(mShaderProgram, name.data());
	glUniformMatrix4fv(location, 1, GL_FALSE, &value.column0.x);
}

void ShaderProgram::setVec3(std::string_view name, const glm::vec3& value) const
{
	const auto location = glGetUniformLocation(mShaderProgram, name.data());
	glUniform3fv(location, 1, value_ptr(value));
}

