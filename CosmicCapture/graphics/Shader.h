// Note: This class is inspired, but not copied from the fall 2020 CPSC 453 OpenGL Boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton

#pragma once

#include <string>
#include <GL/glew.h>


enum class shaderTypes {
	VERTEX, FRAGMENT
};


class Shader
{
public:
	Shader(const char* path, shaderTypes type);
	Shader() : mID(0), mType(shaderTypes::VERTEX) {}

	unsigned int getID() const { return mID; }
	void compile();
	void cleanUp() const { glDeleteShader(mID); }

private:
	unsigned int mID;
	shaderTypes mType;

	// Credit to https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
	// for this method
	static std::string readFileContents(const char* fileName);
};

