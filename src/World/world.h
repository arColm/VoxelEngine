
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
		static const float TIME_PER_DAY;
		static const float TICK_RATE;
		float invTickRate;
		float invTimePerDay;


		World(IChunkGenerator* generator, int seed);
		~World();

		void loadChunks(glm::ivec2 origin, int viewDistance);

		void reloadChunks();

		void generateGUI();
		void updateChunkMap();

		void addChunk(int x, int y, std::shared_ptr<Chunk> chunk);

		void tick(float deltaTime);

		float getCurrentTime();
		float getTotalTime();

		std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>> chunk_map;
		std::mutex chunkMap_mutex; //todo: make this private
	private:

		int seed;
		IChunkGenerator* generator;
		std::queue<std::shared_ptr<Chunk>> chunkQueue;
		std::queue<std::shared_ptr<Chunk>> chunkUnloadingQueue;
		std::mutex chunkQueue_mutex;
		std::mutex chunkUnloadingQueue_mutex;

		boost::signals2::connection chunkLoaderConnection;

		float totalTime;

		float accumulatedDeltaTime;
	};
}


#endif