#pragma once

#include "Entity.h"
#include "input.h"

#include <optional>


class SpikeTrap : public Entity
{
public:
	SpikeTrap();

	void attachPhysics(Physics&) override {}
	void draw(Physics&, const ShaderProgram&, const Camera&, bool) override;
	void simulate(Physics&) override;
	void cleanUpPhysics() override;

	void attachOwningVehicle(PxVehicleDrive4W* vehicle) { mOwnerVehicle = vehicle; }
	void attachAffectedVehicle(PxVehicleDrive4W* vehicle) { mAffectedVehicle = vehicle; }
	bool processInput(const std::map<MovementFlags, bool>&, Physics&);

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

