#pragma once

#include <map>

#include "Entity.h"
#include "physics/Physics.h"
#include "input.h"
#include "physics/VehicleMovement.h"


extern float riseRateAccel;
extern float riseRateBrake;
extern float riseRateHandBrake;
extern float riseRateSteerLeft;
extern float riseRateSteerRight;
extern float fallRateAccel;
extern float fallRateBrake;
extern float fallRateHandBrake;
extern float fallRateSteerLeft;
extern float fallRateSteerRight;
extern float gSteerVsForwardSpeedData1A;
extern float gSteerVsForwardSpeedData1B;
extern float gSteerVsForwardSpeedData2A;
extern float gSteerVsForwardSpeedData2B;
extern float gSteerVsForwardSpeedData3A;
extern float gSteerVsForwardSpeedData3B;
extern float gSteerVsForwardSpeedData4A;
extern float gSteerVsForwardSpeedData4B;


class Vehicle : public Entity
{
public:

	Vehicle(std::shared_ptr<Camera> camera, int playerNum, std::string modelPath, std::string bodyTexturePath, std::string tireTexturePath);

	void attachPhysics(Physics& instance) override;

	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) override;
	void simulate(Physics& instance) override;
	void processInput(const std::map<MovementFlags, bool>& inputs);
	float getVelocity();
	bool isReversing();

	void cleanUpPhysics() override;
	[[nodiscard]] PxVehicleDrive4W* getVehicle() const { return mVehicle4W; }

private:
	PxVehicleDrive4W* mVehicle4W;
	bool mIsVehicleInAir = true;
	bool mInReverseMode = false;
	int player;
	VehicleMovement movement;
	std::unique_ptr<Model> wheel1;
	std::unique_ptr<Model> wheel2;
	std::unique_ptr<Model> wheel3;
	std::unique_ptr<Model> wheel4;
};
