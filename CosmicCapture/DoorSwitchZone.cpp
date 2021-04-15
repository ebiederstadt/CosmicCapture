#include "DoorSwitchZone.h"

DoorSwitchZone::DoorSwitchZone(std::shared_ptr<Camera> camera) :
	Entity("models/dropoff_zone.ply", "textures/fancy.jpg", camera)
{

}

void DoorSwitchZone::attachPhysics(Physics& instance)
{

	
}

void DoorSwitchZone::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth)
{

}

void DoorSwitchZone::simulate(Physics& instance)
{

	if (State::arenaSwitchReady)
	{
		State::arenaTimer += 1;

		if (State::arenaTimer >= AFFECTION_TIME)
		{
			State::arenaSwitch = true;
			State::arenaSwitchReady = false;
		}
	}
}

void DoorSwitchZone::cleanUpPhysics()
{
	PX_RELEASE(State::doorSwitchPickupTriggerBody);
	PX_RELEASE(State::doorSwitchPickupMarkerBody);
}
