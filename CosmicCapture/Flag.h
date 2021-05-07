#pragma once

#include "Entity.h"
#include "GlobalState.h"


class Flag : public Entity
{
public:
	Flag();

	void attachPhysics(Physics&) override;
	void draw(Physics&, const ShaderProgram&, const Camera&, bool) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;
	void resetLastPos();

private:
	std::unique_ptr<Model> mFlagBody;
	PxVec3 lastPos;
};

