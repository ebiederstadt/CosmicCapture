// Note: This class is inspired, but not copied from the fall 2020 CPSC 453 OpenGL Boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton

#pragma once

#include <GL/glew.h>

class VertexArray
{
public:
	VertexArray() { glGenVertexArrays(1, &mID); }
	~VertexArray() { glDeleteVertexArrays(1, &mID); }

	void bind() const { glBindVertexArray(mID); }

private:
	unsigned int mID;
	int mComponentSize = 3;
};

