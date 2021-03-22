// Note: This class is inspired, but not copied from the fall 2020 CPSC 453 OpenGL Boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton

#pragma once

#include <vector>
#include <array>
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


// Simplified form of CPU geometry, to be used in a 2D context
struct GUIGeometry
{
	GUIGeometry();

	std::array<glm::vec2, 4> vertices;
};

class GpuGeometry 
{
public:
	GpuGeometry();
	
	/// <summary>
	/// Upload data to openGL Buffers on the GPU
	/// </summary>
	/// <param name="cpuGeom">CPU Geometry to use</param>
	/// <param name="usage">OpenGL Usage Hint.</param>
	void uploadData(const CpuGeometry& cpuGeom, unsigned int usage);
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

class GUIGPUGeometry
{
public:
	GUIGPUGeometry();

	void uploadData(const GUIGeometry& geom) const;
	void drawData() const;

private:
	VertexArray mVertexArray;
	VertexBuffer mVertexBuffer;

	void bind() const { mVertexArray.bind(); }
};
