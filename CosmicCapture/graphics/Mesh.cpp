#include "Mesh.h"

Mesh::Mesh(const CpuGeometry& cpuGeometry, const Texture& texture) :
	mTexture(texture)
{
	mGeometry.uploadData(cpuGeometry);
}


void Mesh::draw() const
{
	mGeometry.drawData();
}
