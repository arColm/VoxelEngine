

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
		int maxChunksLoaded = (viewDistance + 1) * (viewDistance + 2) * 2;
		//unload chunks?
		if (World::chunkLoader->chunk_map.size() > maxChunksLoaded) {
			//unload chunks
			std::lock_guard<std::mutex> lock(chunkMap_mutex); //consider only locking when erasing
			for (auto it = World::chunkLoader->chunk_map.begin(); it != World::chunkLoader->chunk_map.end(); )
			{
				auto chunk = it->second;
				if (chunk->getX() - origin.x + chunk->getZ() - origin.y > viewDistance) {
					it = World::chunkLoader->chunk_map.erase(it);
				}
				else {
					it++;
				}
			}
		}
		std::vector<std::thread> threads;
		std::queue<std::shared_ptr<VoxelEngine::Chunk>> newChunks;
		std::mutex newChunk_mutex;
		viewDistance++;
		for (int u = viewDistance; u >= -viewDistance; u--) {
			for (int v = abs(u) - viewDistance; abs(u) + v <= viewDistance; v++) {
				glm::ivec2 chunkLoc(origin.x + u, origin.y + v);
				if (World::chunkLoader->chunk_map.find(chunkLoc) == World::chunkLoader->chunk_map.end()) {
					threads.push_back(std::thread(
						[&](int x, int y, std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunkMap, bool isRendered) {
						auto chunk = generator->generateChunk(x, y);
						if (isRendered) {
							std::lock_guard<std::mutex> lock(newChunk_mutex);
							newChunks.push(chunk);
						}
						std::lock_guard<std::mutex> lock(chunkMap_mutex);
						World::chunkLoader->addChunk(x, y, chunk);
						}, chunkLoc.x, chunkLoc.y, std::ref(chunkLoader->chunk_map),
							!(v == abs(u) - viewDistance || v == viewDistance - abs(u))));
				}
				else {
					auto chunk = World::chunkLoader->chunk_map.find(chunkLoc)->second;
					if (!chunk->getIsRendered()) {
						threads.push_back(
							std::thread([&](std::shared_ptr<VoxelEngine::Chunk>chunk, int viewDistance) {
								std::lock_guard<std::mutex> lock(newChunk_mutex);
								newChunks.push(chunk);
							}, chunk, viewDistance));
					}

				}
			}
		}
		for (auto& t : threads) {
			t.join();
		}
		threads.clear();

		while (!newChunks.empty()) {
			std::cout << "AAA" << std::endl;
			auto chunk = newChunks.front(); newChunks.pop();
			threads.push_back(
				std::thread([&](std::shared_ptr<VoxelEngine::Chunk> chunk, std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunkMap) {
					chunk->load(&chunkMap);
					std::lock_guard<std::mutex> lock(chunkQueue_mutex);
					chunkQueue.push(chunk);
				}, chunk, std::ref(chunkLoader->chunk_map)));
		}
		for (auto& t : threads) {
			t.join();
		}
		std::cout << chunkLoader->chunk_map.size() << std::endl;
		//int maxChunksLoaded = viewDistance * (viewDistance + 1) * 2;
		////unload chunks?
		//if (World::chunkLoader->chunk_map.size() > maxChunksLoaded) {
		//	//unload chunks
		//	for (auto it = World::chunkLoader->chunk_map.begin(); it != World::chunkLoader->chunk_map.end(); )
		//	{
		//		auto chunk = it->second;
		//		if (chunk->getX() - origin.x + chunk->getZ() - origin.y > viewDistance) {
		//			it = World::chunkLoader->chunk_map.erase(it);
		//		}
		//		else {
		//			it++;
		//		}
		//	}
		//	std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>> newChunkMap;
		//	for (int u = viewDistance; u >= -viewDistance; u--) {
		//		for (int v = abs(u)-viewDistance; abs(u)+v <= viewDistance; v++) {
		//			glm::ivec2 chunkLoc(origin.x + u, origin.y + v);
		//			if (World::chunkLoader->chunk_map.empty() || World::chunkLoader->chunk_map.find(chunkLoc) == World::chunkLoader->chunk_map.end()) {
		//				std::thread t([&](int x, int y, std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunkMap) {
		//					auto chunk = generator->generateChunk(x, y);
		//					chunk.get()->load(&chunkMap);
		//					std::lock_guard<std::mutex> lock(chunkQueue_mutex);
		//					chunkQueue.push(chunk);
		//					}, chunkLoc.x, chunkLoc.y, std::ref(chunkLoader->chunk_map));
		//				t.detach();
		//				//threads.push_back(t);
		//				//newChunkMap[chunkLoc] = generator->generateChunk(chunkLoc.x, chunkLoc.y);
		//			}
		//		}
		//	}
		//	//for (auto& t : threads) {
		//	//	t.join();
		//	//}
		//	//World::chunkLoader->chunk_map = newChunkMap; //probably inefficient
		//}
		//else {
		//	for (int u = viewDistance; u >= -viewDistance; u--) {
		//		for (int v = abs(u) - viewDistance; abs(u)+v <= viewDistance; v++) {
		//			glm::ivec2 chunkLoc(origin.x + u, origin.y + v);
		//			if (World::chunkLoader->chunk_map.find(chunkLoc)==World::chunkLoader->chunk_map.end()) {

		//				//std::shared_ptr<Chunk> chunk = generator->generateChunk(chunkLoc.x, chunkLoc.y);
		//				//World::chunkLoader->addChunk(chunkLoc.x, chunkLoc.y, chunk);
		//				std::thread t([&](int x, int y, std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunkMap) {
		//					auto chunk = generator->generateChunk(x, y);
		//					chunk.get()->load(&chunkMap);
		//					std::lock_guard<std::mutex> lock(chunkQueue_mutex);
		//					chunkQueue.push(chunk);
		//					}, chunkLoc.x, chunkLoc.y, std::ref(chunkLoader->chunk_map));
		//				t.detach();
		//			}
		//		}
		//	}
		//	//for (auto& t : threads) {
		//	//	t.join();
		//	//}
		//}


		//World::chunkLoader->loadChunks();

	}
	void World::renderChunks() {
		if (!chunkQueue.empty()) {
			std::lock_guard<std::mutex> lock(chunkQueue_mutex);
			while (!chunkQueue.empty()) {
				auto chunk = chunkQueue.front();
				chunkQueue.pop();
				chunk->loadBlocks();
			}
		}
		std::lock_guard<std::mutex> lock(chunkMap_mutex);
		World::chunkLoader->renderChunks();
	}



}