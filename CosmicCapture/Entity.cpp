#include "Entity.h"

Entity::Entity(const char* modelPath, const char* texturePath)
{
	mGeometry = std::make_unique<Model>(modelPath, texturePath);
}
