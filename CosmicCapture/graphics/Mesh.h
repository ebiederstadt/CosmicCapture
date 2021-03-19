#pragma once

#include "Geometry.h"
#include "Texture.h"


class Mesh
{
public:
	Mesh(const CpuGeometry& cpuGeometry, unsigned int usage);

	void draw() const;

private:
	GpuGeometry mGeometry{};
};

