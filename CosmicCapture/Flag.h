#pragma once

#include "Entity.h"

class Flag : public Entity
{
public:
	Flag(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera);

	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

private:
	std::unique_ptr<Model> mFlagBody;
};

