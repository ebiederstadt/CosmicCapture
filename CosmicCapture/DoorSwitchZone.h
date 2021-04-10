#pragma once
#include "Entity.h"
#include "GlobalState.h"
#include "physics/VehicleFilterShader.h"
class DoorSwitchZone : public Entity {
public:
	DoorSwitchZone(int doorSwitchZoneNum);
	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

	float affectionTimer = 0;
	constexpr static float AFFECTION_TIME = 30.0f * 60.0f; // The boost will speed up the player for 10 second
private:
	int doorSwitchZoneNum;
};