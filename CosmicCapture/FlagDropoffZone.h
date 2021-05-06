#pragma once

#include "Entity.h"

class FlagDropoffZone : public Entity
{
public:
	FlagDropoffZone(int playerNum);

	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth) override;
	void simulate(Physics& instance) override {}
	void cleanUpPhysics() override;

private:
	int playerNum;
};

