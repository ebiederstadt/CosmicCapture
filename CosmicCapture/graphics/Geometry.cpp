#include "Geometry.h"

#include <fmt/format.h>


GpuGeometry::GpuGeometry():
	mVertBuffer(0),
	mNormBuffer(1),
	mTextureBuffer(2, 2)
{}

void GpuGeometry::uploadData(const CpuGeometry& cpuGeom, const unsigned int usage)
{
	bind();
	
	mVertBuffer.uploadData(sizeof(glm::vec3) * cpuGeom.vertices.size(), cpuGeom.vertices.data(), usage);
	mNormBuffer.uploadData(sizeof(glm::vec3) * cpuGeom.normals.size(), cpuGeom.normals.data(), usage);
	mTextureBuffer.uploadData(sizeof(glm::vec2) * cpuGeom.texCoords.size(), cpuGeom.texCoords.data(), usage);

	mElementArray.uploadData(sizeof(int) * cpuGeom.indices.size(), (void *) cpuGeom.indices.data(), usage);

	mNumElements = static_cast<int>(cpuGeom.indices.size());
}


void GpuGeometry::drawData() const
{
	bind();
	glDrawElements(GL_TRIANGLES, mNumElements, GL_UNSIGNED_INT, nullptr);
}

