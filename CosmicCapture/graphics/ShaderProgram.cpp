#include "ShaderProgram.h"

#include <stdexcept>
#include <fmt/format.h>


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


