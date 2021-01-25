#include "Geometry.h"


void GpuGeometry::uploadData(const CpuGeometry& cpuGeom)
{
	VAO.bind();

	vertBuffer.uploadData(sizeof(glm::vec3) * cpuGeom.vertices.size(), cpuGeom.vertices.data(), GL_STATIC_DRAW);
	colBuffer.uploadData(sizeof(glm::vec3) * cpuGeom.cols.size(), cpuGeom.cols.data(), GL_STATIC_DRAW);

	EAO.uploadData(sizeof(int) * cpuGeom.indices.size(), (void *) cpuGeom.indices.data(), GL_STATIC_DRAW);

	mNumElements = static_cast<int>(cpuGeom.indices.size());
}


void GpuGeometry::drawData() const
{
		glBindVertexArray(static_cast<unsigned int>(EAO));
		glDrawElements(GL_TRIANGLES, mNumElements, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
}
