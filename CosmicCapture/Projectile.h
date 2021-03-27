#pragma once
#include "Entity.h"
#include "GlobalState.h"
#include "physics/Physics.h"
#include "physics/VehicleFilterShader.h"


class Projectile : public Entity
{
	public:
		Projectile(std::shared_ptr<Camera> camera);
		void attachPhysics(Physics& instance) override;
		void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) override;
		void simulate(Physics& instance) override;
		void cleanUpPhysics() override;

		void attachVehicle(PxVehicleDrive4W* vehicle) { mVehicle = vehicle; }

		bool shouldBeDeleted = false; // Set to true when the projectile should be removed from the world

	private:
		PxVehicleDrive4W* mVehicle;
		PxRigidDynamic* mBody;

		bool active = false;
		float activationTimer = 0;
		constexpr static float ACTIVATION_TIME = 5.0f * 60.0f; // The projectile should only exist in the game world for ~5 seconds before being deleted
};

