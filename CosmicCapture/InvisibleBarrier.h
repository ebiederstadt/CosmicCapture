#pragma once
#include "Entity.h"
#include "GlobalState.h"

class InvisibleBarrier : public Entity
{
public:

	InvisibleBarrier(int type);

	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

private:
	std::vector<PxRigidStatic*> barrierBodies;
	int barrierType;
};

