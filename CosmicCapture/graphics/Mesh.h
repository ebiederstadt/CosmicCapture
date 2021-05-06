#pragma once

#include "Geometry.h"


class Mesh
{
public:
	Mesh(const CpuGeometry& cpuGeometry);

	void draw() const;

private:
	GpuGeometry mGeometry{};
};

