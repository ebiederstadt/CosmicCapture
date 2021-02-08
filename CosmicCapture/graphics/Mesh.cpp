#include "Mesh.h"

Mesh::Mesh(const CpuGeometry& cpuGeometry, const Texture& texture, const unsigned int usage) :
	mTexture(texture)
{
	mGeometry.uploadData(cpuGeometry, usage);
}


void Mesh::draw() const
{
	mGeometry.drawData();
}
