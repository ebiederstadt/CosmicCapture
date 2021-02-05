#pragma once

#include <glm/glm.hpp>

class GraphicsCamera
{
public:
	GraphicsCamera();
	
	glm::mat4 viewMatrix{};
	glm::mat4 projectionMatrix{};
};

