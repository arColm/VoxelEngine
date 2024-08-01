

#include "src/World/world.h"
#include "Camera.h"
#include <thread>


namespace VoxelEngine {
	World::World(IChunkGenerator* generator,int seed) {
		World::generator = generator;
		World::chunkLoader = std::make_unique<ChunkLoader>();
		World::chunkLoaderConnection = mainCamera->enterNewChunkEvent.connect(
			boost::bind(&World::loadChunks,this, boost::placeholders::_1,boost::placeholders::_2));
		World::seed = seed;
	}

	World::~World() {
		World::chunkLoaderConnection.disconnect();
		std::cout << "disconnected";
	}

	void World::loadChunks(glm::ivec2 origin, int viewDistance) {

		int maxChunksLoaded = viewDistance * (viewDistance + 1)*5;
		//std::vector<std::thread> threads;
		//unload chunks?
		if (World::chunkLoader->chunk_map.size() > maxChunksLoaded) {
			std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>> newChunkMap;
			for (int u = viewDistance; u >= -viewDistance; u--) {
				for (int v = abs(u)-viewDistance; abs(u)+v <= viewDistance; v++) {
					glm::ivec2 chunkLoc(origin.x + u, origin.y + v);
					if (World::chunkLoader->chunk_map.empty() || World::chunkLoader->chunk_map.find(chunkLoc) != World::chunkLoader->chunk_map.end()) {
						newChunkMap[chunkLoc] = World::chunkLoader->chunk_map[chunkLoc];
					}
					else {
						//threads.push_back(std::thread([&](int x, int y, std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunkMap) {
						//	newChunkMap[chunkLoc] = generator->generateChunk(x, y);
						//}, chunkLoc.x, chunkLoc.y, std::ref(newChunkMap)));
						newChunkMap[chunkLoc] = generator->generateChunk(chunkLoc.x, chunkLoc.y);
					}
				}
			}
			//for (auto& t : threads) {
			//	t.join();
			//}
			World::chunkLoader->chunk_map = newChunkMap; //probably inefficient
		}
		else {
			for (int u = viewDistance; u >= -viewDistance; u--) {
				for (int v = abs(u) - viewDistance; abs(u)+v <= viewDistance; v++) {
					glm::ivec2 chunkLoc(origin.x + u, origin.y + v);
					if (World::chunkLoader->chunk_map.empty() || World::chunkLoader->chunk_map.find(chunkLoc)==World::chunkLoader->chunk_map.end()) {

						std::shared_ptr<Chunk> chunk = generator->generateChunk(chunkLoc.x, chunkLoc.y);
						World::chunkLoader->addChunk(chunkLoc.x, chunkLoc.y, chunk);
						//threads.push_back(std::thread([&](int x, int y) {
						//	std::shared_ptr<Chunk> chunk = generator->generateChunk(x, y);
						//	World::chunkLoader->addChunk(x, y, chunk);
						//}, chunkLoc.x, chunkLoc.y));
					}
				}
			}
			//for (auto& t : threads) {
			//	t.join();
			//}
		}


		World::chunkLoader->loadChunks();

	}
	void World::renderChunks() {
		World::chunkLoader->renderChunks();
	}


}