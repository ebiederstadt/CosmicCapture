#pragma once

#include <GL\glew.h>


class VertexBuffer
{
public:
	explicit VertexBuffer(GLuint index);
	~VertexBuffer() { glDeleteBuffers(1, &mID); }

	void bind() const { glBindBuffer(GL_ARRAY_BUFFER, mID); }
	void uploadData(GLsizeiptr size, const void* data, GLenum usage) const;

private:
	unsigned int mID{};
	GLuint mIndex; // Corresponds to the location in the shader
};

