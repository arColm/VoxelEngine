

#include "src/World/world.h"
#include "Camera.h"


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
		int maxChunksLoaded = viewDistance * (viewDistance + 1);
		//unload chunks?
		std::cout << World::chunkLoader->chunk_map.size() << std::endl;
		if (World::chunkLoader->chunk_map.size() > maxChunksLoaded) {
			std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>> newChunkMap;
			for (int u = viewDistance; u >= -viewDistance; u--) {
				for (int v = abs(u)-viewDistance; abs(u)+v <= viewDistance; v++) {
					glm::ivec2 chunkLoc(origin.x + u, origin.y + v);
					if (World::chunkLoader->chunk_map.empty() || World::chunkLoader->chunk_map.find(chunkLoc) != World::chunkLoader->chunk_map.end()) {
						newChunkMap[chunkLoc] = World::chunkLoader->chunk_map[chunkLoc];
					}
					else {
						newChunkMap[chunkLoc] = generator->generateChunk(chunkLoc.x, chunkLoc.y);
					}
				}
			}
			World::chunkLoader->chunk_map = newChunkMap; //probably inefficient
		}
		else {
			for (int u = viewDistance; u >= -viewDistance; u--) {
				for (int v = abs(u) - viewDistance; abs(u)+v <= viewDistance; v++) {
					glm::ivec2 chunkLoc(origin.x + u, origin.y + v);
					if (World::chunkLoader->chunk_map.empty() || World::chunkLoader->chunk_map.find(chunkLoc)==World::chunkLoader->chunk_map.end()) {
						std::shared_ptr<Chunk> chunk = generator->generateChunk(chunkLoc.x, chunkLoc.y);
						World::chunkLoader->addChunk(chunkLoc.x, chunkLoc.y, chunk);
					}
				}
			}
		}
		World::chunkLoader->loadChunks();

	}

	void World::renderChunks() {
		World::chunkLoader->renderChunks();
	}



}