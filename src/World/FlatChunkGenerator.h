#ifndef FLATCHUNKGENERATOR_H
#define FLATCHUNKGENERATOR_H

#include <src/World/IChunkGenerator.h>
namespace VoxelEngine {
	class FlatChunkGenerator : public VoxelEngine::IChunkGenerator {
	public:
		Chunk* generateChunk(int x, int z);
	};
}

#endif