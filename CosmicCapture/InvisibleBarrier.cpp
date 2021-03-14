#include "InvisibleBarrier.h"


InvisibleBarrier::InvisibleBarrier(std::shared_ptr<Camera> camera) : 
	Entity("models/blocking_cube", "textures/blank.jpg", camera)
{}

void InvisibleBarrier::attachPhysics(Physics& instance) {}
void InvisibleBarrier::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap) {}
void InvisibleBarrier::simulate(Physics& instance) {}
void InvisibleBarrier::cleanUpPhysics() {}
