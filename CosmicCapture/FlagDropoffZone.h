#pragma once

#include "Entity.h"
#include "GlobalState.h"
#include "physics/VehicleFilterShader.h"

class FlagDropoffZone : public Entity
{
public:
	FlagDropoffZone(std::shared_ptr<Camera> camera, int playerNum);

	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

private:
	int playerNum;
};

