#include "ElementBuffer.h"

void ElementBuffer::uploadData(const GLsizeiptr size, void* data, const GLenum usage) const
{
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}
