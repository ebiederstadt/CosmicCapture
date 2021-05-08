#include "VertexBuffer.h"


VertexBuffer::VertexBuffer(const GLuint index) :
	mIndex(index),
	mComponentSize(3)
{
	glGenBuffers(1, &mID);
}


VertexBuffer::VertexBuffer(const GLuint index, const int componentSize) :
	mIndex(index),
	mComponentSize(componentSize)
{
	glGenBuffers(1, &mID);
}


VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept :
	mID(other.mID),
	mIndex(other.mIndex),
	mComponentSize(other.mComponentSize)
{
	other.mID = 0;
}


void VertexBuffer::uploadData(const GLsizeiptr size, const void* data) const
{
	bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	glVertexAttribPointer(mIndex, mComponentSize, GL_FLOAT, GL_FALSE, mComponentSize * sizeof(float), nullptr);
	glEnableVertexAttribArray(mIndex);
}
