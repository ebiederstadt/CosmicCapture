#include "InvisibleBarrier.h"

#include "OpponentInput.h"
#include "physics/VehicleFilterShader.h"


InvisibleBarrier::InvisibleBarrier(int type) :
	Entity("models/blocking_cube.ply", "textures/white.jpg")
{
	barrierType = type;
}

void InvisibleBarrier::attachPhysics(Physics& instance)
{
	if (barrierType == 1)
	{
		for (int i = 0; i < 26; i++)
		{
			for (int j = 0; j < 26; j++)
			{
				if (worldGrid[i][j] == 0)
				{
					PxShape* barrier = instance.gPhysics->createShape(PxBoxGeometry(12.5f, 12.5f, 12.5f),
					                                                  *instance.gMaterial, true);
					barrier->setSimulationFilterData(
						PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
					PxVec3 p(i * 25.f - 325.f + 12.5f, 0.f, j * 25.f - 325.f + 12.5f);
					barrierBodies.push_back(instance.gPhysics->createRigidStatic(PxTransform(p)));
					barrierBodies.back()->attachShape(*barrier);
					barrier->release();
					instance.gScene->addActor(*barrierBodies.back());
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < 26; i++)
		{
			for (int j = 0; j < 26; j++)
			{
				if ((i == 0 && j >= 0) || (i >= 0 && j == 0) || (i == 25 && j >= 0) || (i >= 0 && j == 25))
				{
					PxShape* barrier = instance.gPhysics->createShape(PxBoxGeometry(12.5f, 12.5f, 12.5f),
					                                                  *instance.gMaterial, true);
					barrier->setSimulationFilterData(
						PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
					PxVec3 p(i * 25.f - 325.f + 12.5f, 0.f, j * 25.f - 325.f + 12.5f);
					barrierBodies.push_back(instance.gPhysics->createRigidStatic(PxTransform(p)));
					barrierBodies.back()->attachShape(*barrier);
					barrier->release();
					instance.gScene->addActor(*barrierBodies.back());
				}
			}
		}
	}
}

void InvisibleBarrier::draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth)
{
}

void InvisibleBarrier::simulate(Physics& instance)
{
}

void InvisibleBarrier::cleanUpPhysics()
{
	for (int i = 0; i < barrierBodies.size(); i++)
	{
		PX_RELEASE(barrierBodies[i]);
	}
}
