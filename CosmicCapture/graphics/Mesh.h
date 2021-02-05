#pragma once

#include "Geometry.h"
#include "Texture.h"


class Mesh
{
public:
	Mesh(const CpuGeometry& cpuGeometry, const Texture& texture);

	void draw() const;

private:
	GpuGeometry mGeometry{};
	Texture mTexture;
};

