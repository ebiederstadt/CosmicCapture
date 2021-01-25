#pragma once

#include <GL\glew.h>


class ElementBuffer
{
public:
	ElementBuffer() { glGenBuffers(1, &mID); }

	explicit  operator unsigned int() const { return mID; }

	void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID); }
	void uploadData(GLsizeiptr size, void* data, GLenum usage) const;

private:
	unsigned int mID;
};

