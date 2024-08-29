

#include "ChunkLoader.h"
#include <Camera.h>
#include <thread>

namespace VoxelEngine{
	//ChunkLoader::ChunkLoader() { 

	//}

	//ChunkLoader::~ChunkLoader() { 

	//}

	////void ChunkLoader::loadChunks() {
	////	std::vector < std::thread> threads;

	////	for(const auto& pair : chunk_map) {
	////		threads.push_back(std::thread(&Chunk::load, pair.second, &chunk_map));
	////	}
	////	for (auto& t : threads) {
	////		t.join();
	////	}
	////}

	//void ChunkLoader::addChunk(int x, int y, std::shared_ptr<Chunk> chunk) {
	//	world->chunk_map.insert_or_assign(glm::ivec2(x, y), chunk);
	//}
	//void ChunkLoader::removeChunk(int x, int y) {

	//}

}