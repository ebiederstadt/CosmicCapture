#pragma once
#include "Entity.h"
#include "GlobalState.h"
#include "physics/VehicleFilterShader.h"
class GridMarker : public Entity
{
public:
	GridMarker(std::shared_ptr<Camera> camera, PxVec3 pos);

	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;
	void setPosition(PxVec3 pos);
private:
	PxVec3 position;
	PxRigidStatic* markerBody[400];
};


