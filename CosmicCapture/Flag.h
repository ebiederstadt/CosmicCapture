#pragma once

#include "Entity.h"
#include "GlobalState.h"

class Flag : public Entity
{
public:
	Flag(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera);

	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

	void attachVehicle(PxVehicleDrive4W* vehicle) { mVehicles.push_back(vehicle); }

private:
	std::unique_ptr<Model> mDropOffZone;
	std::vector<PxVehicleDrive4W*> mVehicles;
};

