// Note: This class is inspired, but not copied from the fall 2020 CPSC 453 OpenGL Boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton

#pragma once

#include <GL\glew.h>


class ElementBuffer
{
public:
	ElementBuffer() { glGenBuffers(1, &mID); }

	// No copying allowed
	ElementBuffer(const ElementBuffer&) = delete;
	ElementBuffer operator=(ElementBuffer other) = delete;

	// Moving is allowed
	ElementBuffer(ElementBuffer&& other) noexcept;

	explicit operator unsigned int() const { return mID; }

	void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID); }
	void uploadData(GLsizeiptr size, void* data, GLenum usage) const;

private:
	unsigned int mID{};
};

