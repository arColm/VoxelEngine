
#ifndef ICHUNKGENERATOR_H
#define ICHUNKGENERATOR_H

#include <src/rendering/Chunk.h>

namespace VoxelEngine {
	class IChunkGenerator {
	public:
		virtual Chunk* generateChunk(int x, int z);
		virtual ~IChunkGenerator();
	};
}


#endif