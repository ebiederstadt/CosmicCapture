#pragma once

#include <glm/glm.hpp>


class Camera
{
public:
	Camera();

	glm::mat4 viewMatrix{};
	glm::mat4 projectionMatrix{};
};

