// Note: This class is inspired, but not copied from the fall 2020 CPSC 453 OpenGL Boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton

#pragma once

#include <GL/glew.h>

class VertexArray
{
public:
	VertexArray();
	~VertexArray() { glDeleteVertexArrays(1, &mID); }

	// No copies allowed (copies of the class don't make sense)
	VertexArray(const VertexArray&) = delete;
	VertexArray operator=(VertexArray) = delete;

	// Moving is allowed
	VertexArray(VertexArray&&) noexcept;

	void bind() const { glBindVertexArray(mID); }

private:
	unsigned int mID{};
};

