#include "Geometry.h"

#include <stdexcept>
#include <fmt/format.h>


void GpuGeometry::uploadData(const CpuGeometry& cpuGeom)
{
#ifdef _DEBUG
	if (cpuGeom.cols.size() != cpuGeom.vertices.size())
		throw std::runtime_error(fmt::format(
			"Number of indices does not match the number of colors ({} != {})",
			cpuGeom.vertices.size(),
			cpuGeom.cols.size())
		);
#endif

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
