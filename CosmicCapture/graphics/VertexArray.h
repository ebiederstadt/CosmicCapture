#pragma once

#include <GL/glew.h>

class VertexArray
{
public:
	VertexArray() { glGenBuffers(1, &mID); }
	~VertexArray() { glDeleteVertexArrays(1, &mID); }

	void bind() const { glBindVertexArray(mID); }

private:
	unsigned int mID{};
};

