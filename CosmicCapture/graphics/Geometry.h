#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"


struct CpuGeometry
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> cols;

	std::vector<int> indices;
};


class GpuGeometry
{
public:
	void uploadData(const CpuGeometry& cpuGeom);
	void drawData() const;

private:
	VertexArray VAO;

	VertexBuffer vertBuffer = VertexBuffer(0);
	VertexBuffer colBuffer = VertexBuffer(1);

	ElementBuffer EAO;

	int mNumElements = 0;
};
