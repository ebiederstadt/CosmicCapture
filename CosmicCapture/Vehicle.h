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

	Vehicle(int, std::string, std::string, std::string );

	void attachPhysics(Physics&) override;

	void draw(Physics&, const ShaderProgram&, const Camera&, bool) override;
	void simulate(Physics&) override;

	// General input method
	void processInput(const InputInfo&);

	// AI input method
	void processInput(const std::map<MovementFlags, bool>&);

	float getVelocity() const;
	bool isReversing() const;

	void cleanUpPhysics() override;
	[[nodiscard]] PxVehicleDrive4W* getVehicle() const { return mVehicle4W; }

	void setHuman(bool, int controllerNum = 0);
	bool isHuman = false;
	bool useKeyboard = false;
	bool useController = false;
	int controllerNumber = 0;

	bool ready = false;

private:
	void moveVehicle(const std::map<MovementFlags, bool>&);

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

