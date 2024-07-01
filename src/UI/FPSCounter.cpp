#include "src/UI/FPSCounter.h"
#include <iostream>


namespace VoxelEngine::FPSCounter {

	float timeSinceLastFrame = 0.f;
	int numFramesPassed = 0;
	int framesPerUpdate = 240;

	void updateFPS(float deltaTime) {
		timeSinceLastFrame += deltaTime;
		numFramesPassed++;

		if (numFramesPassed >= framesPerUpdate) {
			numFramesPassed = 0;
			std::cout << ((float)framesPerUpdate / timeSinceLastFrame) << '\n';
			timeSinceLastFrame = 0.f;
		}
	}
}