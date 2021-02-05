#include "VertexArray.h"


VertexArray::VertexArray()
{
	glGenVertexArrays(1, &mID);
	bind();
}

VertexArray::VertexArray(VertexArray&& other) noexcept :
	mID(other.mID)
{
	other.mID = 0;
}
