#include "Entity.h"
#include "GlobalState.h"
#include "physics/VehicleFilterShader.h"
class FlagDropoffZone : public Entity
{
public:
	FlagDropoffZone(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera, int playerNum);

	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

	void attachVehicle(PxVehicleDrive4W* vehicle) { mVehicles.push_back(vehicle); }

private:
	std::vector<PxVehicleDrive4W*> mVehicles;
	int playerNum;
};

