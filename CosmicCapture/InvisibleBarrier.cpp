#include "InvisibleBarrier.h"


InvisibleBarrier::InvisibleBarrier(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera, int type) : 
	Entity("models/blocking_cube.ply", "textures/blank.jpg", shaderProgram, camera)
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
		int j = 0;
		for (int i = 0; i < 36; i++) {
			PxShape* barrier = instance.gPhysics->createShape(PxBoxGeometry(5.f, 5.f, 5.f), *instance.gMaterial, true);
			barrier->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
			PxVec3 p(i * 10.f - 180.f + 5.f, 0.f, j * 10.f - 180.f + 5.f);
			barrierBodies.push_back(instance.gPhysics->createRigidStatic(PxTransform(p)));
			barrierBodies.back()->attachShape(*barrier);
			barrier->release();
			instance.gScene->addActor(*barrierBodies.back());

			PxShape* barrier2 = instance.gPhysics->createShape(PxBoxGeometry(5.f, 5.f, 5.f), *instance.gMaterial, true);
			barrier2->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
			PxVec3 p2(j * 10.f - 180.f + 5.f, 0.f, i * 10.f - 180.f + 5.f);
			barrierBodies.push_back(instance.gPhysics->createRigidStatic(PxTransform(p)));
			barrierBodies.back()->attachShape(*barrier2);
			barrier2->release();
			instance.gScene->addActor(*barrierBodies.back());
		}
		j = 35;
		for (int i = 0; i < 36; i++) {
			PxShape* barrier = instance.gPhysics->createShape(PxBoxGeometry(5.f, 5.f, 5.f), *instance.gMaterial, true);
			barrier->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
			PxVec3 p(i * 10.f - 180.f + 5.f, 0.f, j * 10.f - 180.f + 5.f);
			barrierBodies.push_back(instance.gPhysics->createRigidStatic(PxTransform(p)));
			barrierBodies.back()->attachShape(*barrier);
			barrier->release();
			instance.gScene->addActor(*barrierBodies.back());

			PxShape* barrier2 = instance.gPhysics->createShape(PxBoxGeometry(5.f, 5.f, 5.f), *instance.gMaterial, true);
			barrier2->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
			PxVec3 p2(j * 10.f - 180.f + 5.f, 0.f, i * 10.f - 180.f + 5.f);
			barrierBodies.push_back(instance.gPhysics->createRigidStatic(PxTransform(p)));
			barrierBodies.back()->attachShape(*barrier2);
			barrier2->release();
			instance.gScene->addActor(*barrierBodies.back());
		}


	}

	
}
void InvisibleBarrier::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap) {}
void InvisibleBarrier::simulate(Physics& instance) {}
void InvisibleBarrier::cleanUpPhysics() {
	for (int i = 0; i < barrierBodies.size(); i++) {
		PX_RELEASE(barrierBodies[i]);
	}
}
