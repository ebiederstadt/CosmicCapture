// Note: This class is inspired, but not copied from the fall 2020 CPSC 453 OpenGL Boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton

#pragma once

#include <GL\glew.h>


class VertexBuffer
{
public:
	explicit VertexBuffer(GLuint index);
	VertexBuffer(GLuint index, int componentSize);
	~VertexBuffer() { glDeleteBuffers(1, &mID); }

	// No copying allowed
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer operator=(VertexBuffer other) = delete;

	// Moving is allowed
	VertexBuffer(VertexBuffer&& other) noexcept;

	void bind() const { glBindBuffer(GL_ARRAY_BUFFER, mID); }
	void uploadData(GLsizeiptr size, const void* data, GLenum usage) const;

private:
	unsigned int mID{};
	GLuint mIndex; // Corresponds to the location in the shader
	int mComponentSize{}; // Number of components per vertex attribute (must be 1, 2, 3, 4)
};

