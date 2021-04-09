#pragma once

#include "Entity.h"

#include <optional>

#include "./input.h"

class SpikeTrap : public Entity
{
public:
	SpikeTrap(const std::shared_ptr<Camera>& camera);

	void attachPhysics(Physics& instance) override {}
	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

	void attachOwningVehicle(PxVehicleDrive4W* vehicle) { mOwnerVehicle = vehicle; }
	void attachAffectedVehicle(PxVehicleDrive4W* vehicle) { mAffectedVehicle = vehicle; }
	bool processInput(const std::map<MovementFlags, bool>& inputs, Physics& instance);

	[[nodiscard]] bool hasOwningVehicle() const { return mOwnerVehicle.has_value(); }
	[[nodiscard]] bool hasAffectedVehicle() const { return mAffectedVehicle.has_value(); }

	int getID() const { return m_id; }

	bool shouldBeDeleted;

private:
	std::optional<PxVehicleDrive4W*> mOwnerVehicle;
	std::optional<PxVehicleDrive4W*> mAffectedVehicle;

	PxRigidStatic* body;

	int m_id;
	
	bool active = false;
	float activationTimer = 0;
	constexpr static float ACTIVATION_TIME = 3.0f * 60.0f; // ~ 3 seconds for the spike trap to become active

	float affectionTimer = 0;
	constexpr static float AFFECTION_TIME = 1.0f * 60.0f; // The spike trap will stop a player for ~ 1 seconds
};

