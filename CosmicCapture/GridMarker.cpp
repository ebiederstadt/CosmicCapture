#include "GridMarker.h"
GridMarker::GridMarker(PxVec3 pos) :
	Entity("models/flag.ply", "textures/pink.jpg")
{
	position = pos;
}

void GridMarker::attachPhysics(Physics& instance)
{
	int counter = 0;
	for (int i = 0; i < 36; i++) {
		for (int j = 0; j < 36; j++) {
			PxVec3 p(State::worldGridCenterCoords[i][j].first, 2.f, State::worldGridCenterCoords[i][j].second);
			PxShape* marker = instance.gPhysics->createShape(PxBoxGeometry(0.1f, 2.f, 0.1f), *instance.gMaterial, true); //create flag shape
			marker->setSimulationFilterData(PxFilterData(COLLISION_FLAG_SCENERY, COLLISION_FLAG_SCENERY_AGAINST, 0, 0));
			markerBody[counter] = instance.gPhysics->createRigidStatic(PxTransform(p));
			markerBody[counter]->attachShape(*marker);
			marker->release();
			instance.gScene->addActor(*markerBody[counter]);
			counter++;
		}
	}


	/*
	PxShape* marker = instance.gPhysics->createShape(PxBoxGeometry(0.1f, 2.f, 0.1f), *instance.gMaterial, true); //create flag shape
	marker->setSimulationFilterData(PxFilterData(COLLISION_FLAG_SCENERY, COLLISION_FLAG_SCENERY_AGAINST, 0, 0));
	markerBody = instance.gPhysics->createRigidStatic(PxTransform(position));
	//create static rigid body - wont move
	markerBody->attachShape(*marker);
	marker->release();
	instance.gScene->addActor(*markerBody);
	*/
	//----------------------------------------------------------

}


void GridMarker::draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth) {
	for (int i = 0; i <  36*36; i++) {
		PxTransform transform = markerBody[i]->getGlobalPose();
		PxMat44 modelMatrix(transform);
		mGeometry->draw(modelMatrix, shader, camera, depth, 1);
	}
}
void GridMarker::simulate(Physics& instance) {
}
void GridMarker::cleanUpPhysics() {
	for (int i = 0; i < 36*36; i++) {
		PX_RELEASE(markerBody[i]);
	}
}

void GridMarker::setPosition(PxVec3 pos) {
	position = pos;
}