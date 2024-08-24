
#ifndef WORLD_H
#define WORLD_H

#include "IChunkGenerator.h"
#include <src/rendering/ChunkLoader.h>
#include "boost/signals2.hpp"
#include <queue>
#include <mutex>
namespace VoxelEngine {

	class World {
	public:
		World(IChunkGenerator* generator, int seed);
		~World();

		void loadChunks(glm::ivec2 origin, int viewDistance);
		void renderChunks();

		void reloadChunks();

	private:

		int seed;
		IChunkGenerator* generator;
		boost::signals2::connection chunkLoaderConnection;
		std::unique_ptr<ChunkLoader> chunkLoader;
		std::queue<std::shared_ptr<Chunk>> chunkQueue;
		std::queue<std::shared_ptr<Chunk>> chunkUnloadingQueue;
		std::mutex chunkQueue_mutex;
		std::mutex chunkUnloadingQueue_mutex;
		std::mutex chunkMap_mutex;

	};
}


#endif