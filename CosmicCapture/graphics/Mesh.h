#pragma once

#include "Geometry.h"


class Mesh
{
public:
	Mesh(const CpuGeometry&);

	void draw() const;

private:
	GpuGeometry mGeometry{};
};

