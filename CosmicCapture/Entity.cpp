#include "Entity.h"

Entity::Entity(const char* modelPath, const char* texturePath)
{
	mGeometry = std::make_unique<Model>(modelPath, texturePath);
}

Entity::Entity(const char* modelPath, const glm::vec4& textureColor)
{
	mGeometry = std::make_unique<Model>(modelPath, textureColor);
}
