#pragma once

class DoorManager {
public:
	void simulate() ;

	float gateDownOffset;
	float gateUpOffset;

	int arenaTimer = 0;
	bool arenaSwitch = false;

private:
	constexpr static float AFFECTION_TIME = 20.0f * 60.0f; // The boost will speed up the player for 10 second
	constexpr static float SWITCH_INTERVAL = 80.f;
	constexpr static float ABS_FRAME_GATE_OFFSET = (120.f / SWITCH_INTERVAL) * 0.3f;  // 120.f because that's what switch interval 0.3f works for
};