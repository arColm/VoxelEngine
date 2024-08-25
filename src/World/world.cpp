

#include "src/World/world.h"
#include "Camera.h"
#include <thread>

#define MULTITHREADED_CHUNKLOADING false


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
		maxChunksLoaded = 1;
		//unload chunks?
		if (World::chunkLoader->chunk_map.size() > maxChunksLoaded) {
			//unload chunks
			std::lock_guard<std::mutex> lock(chunkMap_mutex); //consider only locking when erasing
			for (auto it = chunkLoader->chunk_map.cbegin(); it != chunkLoader->chunk_map.cend();)
			{
				if (abs(it->first.x - origin.x) + abs(it->first.y - origin.y) > viewDistance)
				{
					std::lock_guard<std::mutex> lock(chunkUnloadingQueue_mutex);
					chunkUnloadingQueue.push(it->second);
					it = chunkLoader->chunk_map.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
		std::vector<std::jthread> threads;
		std::queue<std::shared_ptr<VoxelEngine::Chunk>> newChunks;
		std::mutex newChunk_mutex;
		for (int u = viewDistance; u >= -viewDistance; u--) {
			for (int v = abs(u) - viewDistance; abs(u) + v <= viewDistance; v++) {
				glm::ivec2 chunkLoc(origin.x + u, origin.y + v);
				if (World::chunkLoader->chunk_map.find(chunkLoc) == World::chunkLoader->chunk_map.end()) {
					if (MULTITHREADED_CHUNKLOADING) {
						threads.push_back(std::jthread(
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
						bool isRendered = !(v == abs(u) - viewDistance || v == viewDistance - abs(u));
						auto chunk = generator->generateChunk(chunkLoc.x, chunkLoc.y);
						if (isRendered) {
							std::lock_guard<std::mutex> lock(newChunk_mutex);
							newChunks.push(chunk);
						}
						std::lock_guard<std::mutex> lock(chunkMap_mutex);
						World::chunkLoader->addChunk(chunkLoc.x, chunkLoc.y, chunk);
					}

				}
				else {
					auto& chunk = World::chunkLoader->chunk_map.find(chunkLoc)->second;
					if (!chunk->getIsRendered()) {
						if (MULTITHREADED_CHUNKLOADING) {
							threads.push_back(
								std::jthread([&](std::shared_ptr<VoxelEngine::Chunk>chunk, int viewDistance) {
									std::lock_guard<std::mutex> lock(newChunk_mutex);
									newChunks.push(chunk);
								}, chunk, viewDistance));
						}
						else {
							std::lock_guard<std::mutex> lock(newChunk_mutex);
							newChunks.push(chunk);
						}

					}

				}
			}
		}
		for (auto& t : threads) {
			t.join();
		}
		threads.clear();

		while (!newChunks.empty()) {
			auto& chunk = newChunks.front(); newChunks.pop();
			//threads.push_back(
			//	std::jthread([&](std::shared_ptr<VoxelEngine::Chunk> chunk, std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunkMap) {
			//		chunk->load(&chunkMap);
			//		std::lock_guard<std::mutex> lock(chunkQueue_mutex);
			//		chunkQueue.push(chunk);
			//	}, chunk, std::ref(chunkLoader->chunk_map)));

			chunk->load(&chunkLoader->chunk_map);
			std::lock_guard<std::mutex> lock(chunkQueue_mutex);
			chunkQueue.push(chunk);
		}
		for (auto& t : threads) {
			t.join();
		}
		threads.clear();

	}
	void World::reloadChunks() {
		{
			std::lock_guard<std::mutex> lock(chunkMap_mutex);
			std::lock_guard<std::mutex> lock2(chunkUnloadingQueue_mutex);
			for(auto& c : chunkLoader->chunk_map) {
				chunkUnloadingQueue.push(c.second);
			}
			chunkLoader->chunk_map.clear();
		}

		if (!chunkQueue.empty()) {
			std::lock_guard<std::mutex> lock(chunkQueue_mutex);
			while (!chunkQueue.empty()) {
				chunkQueue.pop();
			}
		}
		mainCamera->forceUpdateCurrentChunk();
	}
	void World::renderChunks() {
		if (!chunkUnloadingQueue.empty()) {
			std::lock_guard<std::mutex> lock(chunkUnloadingQueue_mutex);
			while (!chunkUnloadingQueue.empty()) {
				chunkUnloadingQueue.pop();
			}
		}
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

	void World::generateGUI() {
		generator->generateGUI();
	}
	const std::type_info& World::getGeneratorType() {
		return typeid(&generator);
	}


}