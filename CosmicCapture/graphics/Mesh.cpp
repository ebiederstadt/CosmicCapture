#include "Mesh.h"

Mesh::Mesh(const CpuGeometry& cpuGeometry, const unsigned int usage)
{
	mGeometry.uploadData(cpuGeometry, usage);
}


void Mesh::draw() const
{
	mGeometry.drawData();
}
