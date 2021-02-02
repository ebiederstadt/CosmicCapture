#include "Geometry.h"

#include <fmt/format.h>


GpuGeometry::GpuGeometry():
	mVertBuffer(0),
	mNormBuffer(1),
	mTextureBuffer(2, 2)
{}

void GpuGeometry::uploadData(const CpuGeometry& cpuGeom)
{
	bind();
	
	mVertBuffer.uploadData(sizeof(glm::vec3) * cpuGeom.vertices.size(), cpuGeom.vertices.data(), GL_STATIC_DRAW);
	mNormBuffer.uploadData(sizeof(glm::vec3) * cpuGeom.normals.size(), cpuGeom.normals.data(), GL_STATIC_DRAW);
	mTextureBuffer.uploadData(sizeof(glm::vec2) * cpuGeom.texCoords.size(), cpuGeom.texCoords.data(), GL_STATIC_DRAW);

	mElementArray.uploadData(sizeof(int) * cpuGeom.indices.size(), (void *) cpuGeom.indices.data(), GL_STATIC_DRAW);

	mNumElements = static_cast<int>(cpuGeom.indices.size());
}


void GpuGeometry::drawData() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<unsigned int>(mElementArray));
	glDrawElements(GL_TRIANGLES, mNumElements, GL_UNSIGNED_INT, nullptr);
}


void GpuGeometry::bind() const
{
	mVertexArray.bind();
}
