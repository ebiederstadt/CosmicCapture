#pragma once

#include <memory>
#include "graphics/Model.h"
#include "physics/Physics.h"


class Entity
{
public:
	Entity(std::unique_ptr<Model> model) : mGeometry(std::move(model)) {}

	virtual void attachPhysics(Physics& instance) = 0;

	virtual void draw(Physics& instance) = 0;
	virtual void simulate(Physics& instance) = 0;

	virtual void cleanUpPhysics() = 0;

	// Might want to move this somewhere else, more to do with rendering tbh
	static constexpr int MAX_NUM_ACTOR_SHAPES = 128;
	std::unique_ptr<Model> mGeometry;
};

