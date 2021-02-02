// Note: This class is inspired, but not copied from the fall 2020 CPSC 453 OpenGL Boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton

#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"


struct CpuGeometry
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	std::vector<int> indices;
};


class GpuGeometry
{
public:
	GpuGeometry();
	
	void uploadData(const CpuGeometry& cpuGeom);
	void drawData() const;

private:
	VertexArray mVertexArray;

	VertexBuffer mVertBuffer;
	VertexBuffer mNormBuffer;
	VertexBuffer mTextureBuffer;

	ElementBuffer mElementArray;

	int mNumElements = 0;

	void bind() const { mVertexArray.bind(); }
};
