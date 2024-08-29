#ifndef CHUNKLOADER_H
#define CHUNKLOADER_H

#include "Chunk.h"
#include <vector>
#include <glad/glad.h>
#include <unordered_map>
#include <glm/fwd.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include <src/World/world.h>

namespace VoxelEngine {
	//class ChunkLoader {
	//public:
	//	ChunkLoader();
	//	~ChunkLoader();

	//	void addChunk(int x, int y, std::shared_ptr<Chunk> chunk);
	//	void removeChunk(int x, int y);
	//private:

	//	std::shared_ptr<World> world;
	//	boost::signals2::connection enterNewChunkEventConnection;
	//};
}


#endif