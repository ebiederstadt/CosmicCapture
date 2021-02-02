#include "Camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Camera::Camera() :
	viewMatrix(glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, -3.0f})),
	projectionMatrix(glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f))
{}

