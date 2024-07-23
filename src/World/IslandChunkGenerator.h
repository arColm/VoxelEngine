#pragma once

#include <src/World/IChunkGenerator.h>

namespace VoxelEngine {
	class IslandChunkGenerator : public VoxelEngine::IChunkGenerator {
	public:
		std::shared_ptr<Chunk> generateChunk(int x, int z);
	};
}
