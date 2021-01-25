#include "VertexBuffer.h"


VertexBuffer::VertexBuffer(const GLuint index) : mIndex(index)
{
	glGenBuffers(1, &mID);
}

void VertexBuffer::uploadData(GLsizeiptr size, const void* data, GLenum usage) const
{
	bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);

	glVertexAttribPointer(mIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(mIndex);
}
