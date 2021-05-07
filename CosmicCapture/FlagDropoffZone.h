#pragma once

#include "Entity.h"

class FlagDropoffZone : public Entity
{
public:
	FlagDropoffZone(int playerNum);

	void attachPhysics(Physics&) override;
	void draw(Physics&, const ShaderProgram&, const Camera&, bool) override;
	void simulate(Physics&) override {}
	void cleanUpPhysics() override;

private:
	int playerNum;
};

