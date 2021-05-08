#include "Geometry.h"

#include <fmt/format.h>

using namespace glm;

// CPU side geometry

GUIGeometry::GUIGeometry()
{
	vertices = {
	vec2(-1.f, 1.f),
	vec2(-1.f, -1.f),
	vec2(1.0f, 1.0f),
	vec2(1.0f, -1.f)
	};
}

FontGeometry::FontGeometry()
{
	vertices = {
	vec2(-1.f, 1.f),
	vec2(-1.f, -1.f),
	vec2(1.0f, 1.0f),
	vec2(1.0f, -1.f)
	};
}

// GUP side geometry

GpuGeometry::GpuGeometry():
	mVertBuffer(0),
	mNormBuffer(1),
	mTextureBuffer(2, 2)
{}

void GpuGeometry::uploadData(const CpuGeometry& cpuGeom)
{
	bind();
	
	mVertBuffer.uploadData(sizeof(vec3) * cpuGeom.vertices.size(), cpuGeom.vertices.data());
	mNormBuffer.uploadData(sizeof(vec3) * cpuGeom.normals.size(), cpuGeom.normals.data());
	mTextureBuffer.uploadData(sizeof(vec2) * cpuGeom.texCoords.size(), cpuGeom.texCoords.data());

	mElementArray.uploadData(sizeof(int) * cpuGeom.indices.size(), (void *) cpuGeom.indices.data());

	mNumElements = static_cast<int>(cpuGeom.indices.size());
}

void GpuGeometry::drawData() const
{
	bind();
	glDrawElements(GL_TRIANGLES, mNumElements, GL_UNSIGNED_INT, nullptr);
}

GUIGPUGeometry::GUIGPUGeometry() :
	mVertexBuffer(0, 2)
{
}

void GUIGPUGeometry::uploadData(const GUIGeometry& geom) const
{
	bind();
	mVertexBuffer.uploadData(sizeof(vec2) * geom.vertices.size(), geom.vertices.data());
}

void GUIGPUGeometry::drawData() const
{
	bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


FontGpuGeometry::FontGpuGeometry() :
	mVertBuffer(0, 2),
	mTextureBuffer(1, 2)
{
}

void FontGpuGeometry::uploadData(const FontGeometry& geom) const
{
	bind();
	mVertBuffer.uploadData(sizeof(vec2) * geom.vertices.size(), geom.vertices.data());
	mTextureBuffer.uploadData(sizeof(vec2) * geom.texCoords.size(), geom.texCoords.data());
}

void FontGpuGeometry::drawData() const
{
	bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
