#pragma once

#include "Entity.h"
#include "GlobalState.h"
#include "physics/VehicleFilterShader.h"


class Flag : public Entity
{
public:
	Flag(const std::shared_ptr<Camera>& camera);

	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

private:
	std::unique_ptr<Model> mFlagBody;
};

