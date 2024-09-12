

#include "src/World/world.h"
#include "Camera.h"
#include <thread>
#include <imgui.h>
#include <src/rendering/DebugRenderer.h>

#define MULTITHREADED_CHUNKLOADING false


namespace VoxelEngine {

	const float World::TIME_PER_DAY = 30.0f;
	const float World::TICK_RATE = 20.0f;

	World::World(IChunkGenerator* generator,int seed) {
		World::generator = generator;
		World::seed = seed;
		World::totalTime = 0;
		World::accumulatedDeltaTime = 0;
		World::invTickRate = 1.f / TICK_RATE;
		World::invTimePerDay = 1.f / TIME_PER_DAY;

		World::chunkLoaderConnection = mainCamera->enterNewChunkEvent.connect(
			boost::bind(&World::handleEnterNewChunkEvent, this, boost::placeholders::_1, boost::placeholders::_2));
	}

	World::~World() {
		World::chunkLoaderConnection.disconnect();
		std::cout << "disconnected";
	}

	bool isWithinLoadDistance(int loadDistance, glm::ivec2 origin, glm::ivec2 chunkLocation) {
		return chunkLocation.x >= origin.x - loadDistance &&
			chunkLocation.x <= origin.x + loadDistance &&
			chunkLocation.y >= origin.y - loadDistance &&
			chunkLocation.y <= origin.y + loadDistance;
	}

	void World::handleEnterNewChunkEvent(glm::ivec2 origin, int viewDistance) {
		int loadDistance = viewDistance + 2;
		int decorateDistance = loadDistance-1;
		int maxChunksLoaded = (loadDistance) * (loadDistance + 1) * 5;
		
		// 1. check if have to unload any chunks
		if (chunk_map.size() > maxChunksLoaded) {
			std::lock_guard<std::mutex> lock(chunkMap_mutex); //consider only locking when erasing
			for (auto it = chunk_map.cbegin(); it != chunk_map.cend();)
			{
				if (!isWithinLoadDistance(loadDistance,origin,glm::vec2(it->first.x,it->first.y)))
				{
					std::lock_guard<std::mutex> lock(chunkUnloadingQueue_mutex);
					chunkUnloadingQueue.push(it->second);
					it = chunk_map.erase(it);
				}
				else
				{
					++it;
				}
			}

		}

		//2. check if have to add new chunks that are NOT IN MAP[view_distance + 2]

		std::queue<std::shared_ptr<VoxelEngine::Chunk>> newChunks;
		for (int u = origin.x - loadDistance; u <= origin.x + loadDistance; u++) {
			for (int v = origin.y - loadDistance; v <= origin.y + loadDistance; v++) {
				glm::ivec2 chunkLoc(u, v);
				if (chunk_map.find(chunkLoc) == chunk_map.end()) {
					auto chunk = std::make_shared<Chunk>(chunkLoc.x, chunkLoc.y);
					newChunks.push(chunk);
				}
			}
		}


		//3. generate terrain for all chunks in terrain_generation queue
		while (!newChunks.empty()) {
			std::shared_ptr<Chunk> chunk = newChunks.front(); newChunks.pop();
			generator->generateTerrain(chunk);
			//mark as terraingenerated
			chunk->updateState(Chunk::State::TerrainGenerated);
			std::lock_guard<std::mutex> lock(chunkMap_mutex);
			addChunk(chunk->getX(), chunk->getZ(), chunk);
		}


		//4. decorate chunks in map that are marked TerrainGenerated [view_distance + 1] -- corners filled
		for (int u = origin.x - decorateDistance; u <= origin.x + decorateDistance; u++) {
			for (int v = origin.y - decorateDistance; v <= origin.y + decorateDistance; v++) {
				glm::ivec2 chunkLoc(u, v);
				auto chunkIt = chunk_map.find(chunkLoc);

				if (chunkIt == chunk_map.end()) {
					std::cout << "ERROR :: CHUNK NOT LOADED" << std::endl;
					continue;
				}


				//POTENTIAL RACE CONDITION -- chunk may be deleted ?

				auto chunk = chunkIt->second;
				if (chunk->getState() == Chunk::State::TerrainGenerated) {
					generator->decorateChunk(chunk,chunk_map);

					chunk->updateState(Chunk::State::WaitingForMesh);
					newChunks.push(chunk);
				}
			}
		}

		while (!newChunks.empty()) {
			auto chunk = newChunks.front(); newChunks.pop();

			chunk->load(&chunk_map);
			std::lock_guard<std::mutex> lock(chunkQueue_mutex);
			chunkQueue.push(chunk);

		}

		//loadChunks(origin, viewDistance);
	}



	void World::loadChunks(glm::ivec2 origin, int viewDistance) {
		int maxChunksLoaded = (viewDistance + 1) * (viewDistance + 2) * 2;
		//unload chunks?
		if (chunk_map.size() > maxChunksLoaded) {
			//unload chunks
			std::lock_guard<std::mutex> lock(chunkMap_mutex); //consider only locking when erasing
			for (auto it = chunk_map.cbegin(); it != chunk_map.cend();)
			{
				if (abs(it->first.x - origin.x) + abs(it->first.y - origin.y) > viewDistance)
				{
					std::lock_guard<std::mutex> lock(chunkUnloadingQueue_mutex);
					chunkUnloadingQueue.push(it->second);
					it = chunk_map.erase(it);
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
				if (chunk_map.find(chunkLoc) == chunk_map.end()) {
					if (MULTITHREADED_CHUNKLOADING) {
						threads.push_back(std::jthread(
							[&](int x, int y, std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunkMap, bool isRendered) {
							auto chunk = generator->generateChunk(x, y);
							if (isRendered) {
								std::lock_guard<std::mutex> lock(newChunk_mutex);
								newChunks.push(chunk);
							}
							std::lock_guard<std::mutex> lock(chunkMap_mutex);
							addChunk(x, y, chunk);
							}, chunkLoc.x, chunkLoc.y, std::ref(chunk_map),
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
						addChunk(chunkLoc.x, chunkLoc.y, chunk);
					}

				}
				else {
					auto& chunk = chunk_map.find(chunkLoc)->second;
					if (!chunk->getHasMesh()) {
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

			chunk->load(&chunk_map);
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
			for(auto& c : chunk_map) {
				chunkUnloadingQueue.push(c.second);
			}
			chunk_map.clear();
		}

		if (!chunkQueue.empty()) {
			std::lock_guard<std::mutex> lock(chunkQueue_mutex);
			while (!chunkQueue.empty()) {
				chunkQueue.pop();
			}
		}
		mainCamera->forceUpdateCurrentChunk();
	}
	void World::updateChunkMap() {
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
				chunk->generateMesh();
			}
		}
	}


	void World::generateGUI() {
		//generic world gui
		ImGui::Text("Time = %f", getCurrentTime());
		ImGui::Separator();

		//generator gui
		generator->generateGUI();

		if (ImGui::Button("Reload Chunks"))
			reloadChunks();
	}


	void World::addChunk(int x, int y, std::shared_ptr<Chunk> chunk) {
		chunk_map.insert_or_assign(glm::ivec2(x, y), chunk);
	}
	void World::tick(float deltaTime)
	{
		accumulatedDeltaTime += deltaTime;
		while (accumulatedDeltaTime >= invTickRate) {
			//tick all objects for invTickRate time
			totalTime += invTickRate;


			accumulatedDeltaTime -= invTickRate;
		}
	}
	float World::getCurrentTime()
	{
		return std::fmod(totalTime,TIME_PER_DAY);
	}
	float World::getTotalTime() {
		return totalTime;
	}
}