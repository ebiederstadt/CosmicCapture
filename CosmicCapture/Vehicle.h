#pragma once

#include <map>

#include "Entity.h"
#include "physics/Physics.h"
#include "input.h"
#include "physics/VehicleMovement.h"


// TODO: Might want to think about a better place to store these definitions (so they are not longer globals)
inline PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		6.0f, //rise rate eANALOG_INPUT_ACCEL
		6.0f, //rise rate eANALOG_INPUT_BRAKE		
		6.0f, //rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f, //rise rate eANALOG_INPUT_STEER_LEFT
		2.5f, //rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f, //fall rate eANALOG_INPUT_ACCEL
		10.0f, //fall rate eANALOG_INPUT_BRAKE		
		10.0f, //fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f, //fall rate eANALOG_INPUT_STEER_LEFT
		5.0f //fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

inline PxVehiclePadSmoothingData gPadSmoothingData =
{
	{
		6.0f, //rise rate eANALOG_INPUT_ACCEL
		6.0f, //rise rate eANALOG_INPUT_BRAKE		
		6.0f, //rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f, //rise rate eANALOG_INPUT_STEER_LEFT
		2.5f, //rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f, //fall rate eANALOG_INPUT_ACCEL
		10.0f, //fall rate eANALOG_INPUT_BRAKE		
		10.0f, //fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f, //fall rate eANALOG_INPUT_STEER_LEFT
		5.0f //fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

inline PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f, 0.75f,
	5.0f, 0.75f,
	30.0f, 0.125f,
	120.0f, 0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
inline PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);


class Vehicle : public Entity
{
public:
	Vehicle(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera, int playerNum, std::string texturePath);
	void attachPhysics(Physics& instance) override;

	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap) override;
	void simulate(Physics& instance) override;
	void processInput(const std::map<MovementFlags, bool>& inputs);

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
