#pragma once
#include "boost/signals2.hpp"
#include <src/shaders/shader.h>


namespace VoxelEngine {

	namespace DebugRenderer {

		extern boost::signals2::signal<void()> drawDebugEvent;
		void renderDebug();
		void initializeDebugRenderer();
		void releaseResources();

		void drawCube(glm::vec3 pos, glm::vec3 color);
	};
}