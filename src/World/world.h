
#ifndef WORLD_H
#define WORLD_H

#include "IChunkGenerator.h"
#include <src/rendering/ChunkLoader.h>
#include "boost/signals2.hpp"
namespace VoxelEngine {

	class World {
		int seed;
		IChunkGenerator* generator;
		boost::signals2::connection chunkLoaderConnection;
		VoxelEngine::ChunkLoader* chunkLoader;

	public:
		World(IChunkGenerator* generator, int seed);
		~World();

		void loadChunks(glm::ivec2 origin, int viewDistance);

	};
}


#endif