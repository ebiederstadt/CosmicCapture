#include "InvisibleBarrier.h"

#include "Colors.h"


InvisibleBarrier::InvisibleBarrier(int type) : 
	Entity("models/blocking_cube.ply", "textures/blank.jpg")
{
	barrierType = type;
}

void InvisibleBarrier::attachPhysics(Physics& instance) {
	if (barrierType == 1) {
		for (int i = 0; i < 36; i++) {
			for (int j = 0; j < 36; j++) {
				if (State::worldGrid[i][j] == 0) {
					PxShape* barrier = instance.gPhysics->createShape(PxBoxGeometry(5.f, 5.f, 5.f), *instance.gMaterial, true);
					barrier->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
					PxVec3 p(i * 10.f - 180.f + 5.f, 0.f, j * 10.f - 180.f + 5.f);
					barrierBodies.push_back(instance.gPhysics->createRigidStatic(PxTransform(p)));
					barrierBodies.back()->attachShape(*barrier);
					barrier->release();
					instance.gScene->addActor(*barrierBodies.back());
				}
			}
		}
	}
	else {
		for (int i = 0; i < 36; i++) {
			for (int j = 0; j < 36; j++) {
				if ((i == 0 && j >= 0) || (i >= 0 && j == 0) || (i == 35 && j >= 0) || (i >= 0 && j == 35)) {
					PxShape* barrier = instance.gPhysics->createShape(PxBoxGeometry(5.f, 5.f, 5.f), *instance.gMaterial, true);
					barrier->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
					PxVec3 p(i * 10.f - 180.f + 5.f, 0.f, j * 10.f - 180.f + 5.f);
					barrierBodies.push_back(instance.gPhysics->createRigidStatic(PxTransform(p)));
					barrierBodies.back()->attachShape(*barrier);
					barrier->release();
					instance.gScene->addActor(*barrierBodies.back());
				}
			}
		}
	}

	
}
void InvisibleBarrier::draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth) {}
void InvisibleBarrier::simulate(Physics& instance) {}
void InvisibleBarrier::cleanUpPhysics() {
	for (int i = 0; i < barrierBodies.size(); i++) {
		PX_RELEASE(barrierBodies[i]);
	}
}
