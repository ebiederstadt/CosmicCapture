#include "ElementBuffer.h"


ElementBuffer::ElementBuffer(ElementBuffer&& other) noexcept :
	mID(other.mID)
{
	other.mID = 0;
}


void ElementBuffer::uploadData(const GLsizeiptr size, void* data, const GLenum usage) const
{
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}
