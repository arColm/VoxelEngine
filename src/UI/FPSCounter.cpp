#include "src/UI/FPSCounter.h"
#include <iostream>


namespace VoxelEngine::FPSCounter {

	float timeSinceLastUpdate = 0.f;
	int numFramesPassed = 0;
	float secondsPerUpdate = 1.f;
	int framesPerUpdate = 60;

	void updateFPS(float deltaTime) {
		timeSinceLastUpdate += deltaTime;
		numFramesPassed++;

		if (timeSinceLastUpdate>=secondsPerUpdate) {
			std::cout << (numFramesPassed/timeSinceLastUpdate) << '\n';
			numFramesPassed = 0;
			timeSinceLastUpdate -= secondsPerUpdate;
		}
	}
}