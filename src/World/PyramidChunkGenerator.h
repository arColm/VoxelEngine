#ifndef PYRAMIDCHUNKGENERATOR_H
#define PYRAMIDCHUNKGENERATOR_H

#include <src/World/IChunkGenerator.h>
namespace VoxelEngine {
	class PyramidChunkGenerator : public VoxelEngine::IChunkGenerator {
	public:
		std::shared_ptr<Chunk> generateChunk(int x, int z);
	};
}

#endif