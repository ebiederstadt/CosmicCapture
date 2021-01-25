// Note: This class is inspired, but not copied from the fall 2020 CPSC 453 OpenGL Boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton

#pragma once

#include <string>
#include <GL/glew.h>

#include "Shader.h"


class ShaderProgram
{
public:
	ShaderProgram(const char* vertLoc, const char* fragLoc);
	~ShaderProgram() { glDeleteProgram(mShaderProgram); }

	void compile();
	void use() const { glUseProgram(mShaderProgram); }

private:
	unsigned int mShaderProgram;
	
	Shader mVertex;
	Shader mFragment;

};

