#pragma once
#include "Entity.h"
#include "GlobalState.h"
#include "physics/VehicleFilterShader.h"

class InvisibleBarrier : public Entity
{
public:

	InvisibleBarrier(std::shared_ptr<Camera> camera, int type);

	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;
private:
	std::vector<PxRigidStatic*> barrierBodies;
	int barrierType;
};

