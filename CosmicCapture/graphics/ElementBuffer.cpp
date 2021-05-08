#include "ElementBuffer.h"


ElementBuffer::ElementBuffer(ElementBuffer&& other) noexcept :
	mID(other.mID)
{
	other.mID = 0;
}


void ElementBuffer::uploadData(const GLsizeiptr size, void* data) const
{
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
