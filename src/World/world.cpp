

#include "src/World/world.h"
#include "Camera.h"


namespace VoxelEngine {
	World::World(IChunkGenerator* generator,int seed) {
		World::generator = generator;
		ChunkLoader cl;
		World::chunkLoader = &cl;
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
			return;
			std::unordered_map<glm::ivec2, Chunk*> newChunkMap;
			for (int u = viewDistance; u >= -viewDistance; u--) {
				for (int v = abs(u)-viewDistance; abs(u + v) <= viewDistance; v++) {
					glm::ivec2 chunkLoc(origin.x + u, origin.y + v);
					if (World::chunkLoader->chunk_map.empty() || World::chunkLoader->chunk_map.find(chunkLoc) != World::chunkLoader->chunk_map.end()) {
						//newChunkMap[chunkLoc] = World::chunkLoader->chunk_map[chunkLoc];
					}
					else {
						//newChunkMap[chunkLoc] = generator->generateChunk(chunkLoc.x, chunkLoc.y);
					}
				}
			}
		}
		else {
			for (int u = viewDistance; u >= -viewDistance; u--) {
				for (int v = abs(u) - viewDistance; abs(u + v) <= viewDistance; v++) {
					glm::ivec2 chunkLoc(origin.x + u, origin.y + v);
					std::cout << u << '-' << v << std::endl;
					if (World::chunkLoader->chunk_map.empty() || World::chunkLoader->chunk_map.find(chunkLoc)==World::chunkLoader->chunk_map.end()) {
						//World::chunkLoader->chunk_map[chunkLoc] = generator->generateChunk(chunkLoc.x, chunkLoc.y);
						Chunk* chunk = generator->generateChunk(chunkLoc.x, chunkLoc.y);
						World::chunkLoader->addChunk(chunkLoc.x, chunkLoc.y, chunk);
					}
				}
			}
		}

	}



}