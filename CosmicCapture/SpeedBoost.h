#pragma once
#include "Entity.h"
#include "GlobalState.h"
#include "physics/Physics.h"
#include "physics/VehicleFilterShader.h"

class SpeedBoost : public Entity{
public:
	SpeedBoost(std::shared_ptr<Camera> camera);
	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

	void attachVehicle(PxVehicleDrive4W* vehicle) { mVehicles.push_back(vehicle); }
	void detachVehicle() { mVehicles.clear(); }

private:
	std::vector<PxVehicleDrive4W*> mVehicles;

	float affectionTimer = 0;
	constexpr static float AFFECTION_TIME = 1.0f * 60.0f; // The boost will speed up the player for 1 second
};
