#pragma once
#include "Entity.h"
#include "GlobalState.h"

class InvisibleBarrier : public Entity
{
public:

	InvisibleBarrier(int type);

	void attachPhysics(Physics&) override;
	void draw(Physics&, const ShaderProgram&, const Camera&, bool) override;
	void simulate(Physics&) override;
	void cleanUpPhysics() override;

private:
	std::vector<PxRigidStatic*> barrierBodies;
	int barrierType;
};

