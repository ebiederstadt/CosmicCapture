#pragma once
#include "Entity.h"
#include "GlobalState.h"
#include "Input.h"
#include "physics/Physics.h"

class SpeedBoost : public Entity{
public:
	SpeedBoost();
	void attachPhysics(Physics&) override;
	void draw(Physics&, const ShaderProgram&, const Camera&, bool) override;
	void simulate(Physics&) override;
	void cleanUpPhysics() override;

	void attachVehicle(PxVehicleDrive4W* vehicle) { mVehicles.push_back(vehicle); }
	void detachVehicle() { mVehicles.clear(); }

	bool shouldBeDeleted = false;

private:
	std::vector<PxVehicleDrive4W*> mVehicles;

	bool active = false;

	float affectionTimer = 0;
	constexpr static float AFFECTION_TIME = 1.0f * 1.0f; // The boost will speed up the player for 1 frames
};
