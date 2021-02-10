#pragma once

#include <physx/PxPhysicsAPI.h>
#include <glm/glm.hpp>

inline glm::vec3 pxVec2glm(const physx::PxVec3& vec)
{
	return { vec.x, vec.y, vec.z };
}
