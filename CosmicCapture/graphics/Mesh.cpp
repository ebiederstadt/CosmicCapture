#include "Mesh.h"

Mesh::Mesh(const CpuGeometry& cpuGeometry)
{
	mGeometry.uploadData(cpuGeometry);
}


void Mesh::draw() const
{
	mGeometry.drawData();
}
