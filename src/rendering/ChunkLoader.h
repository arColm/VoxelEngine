#ifndef CHUNKLOADER_H
#define CHUNKLOADER_H

#include "Chunk.h"
#include <vector>
#include <glad/glad.h>
#include <unordered_map>
#include <glm/fwd.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

namespace VoxelEngine {
	class ChunkLoader {
	public:
		ChunkLoader(Shader* shader);
		~ChunkLoader();
		void renderChunks();
		void addChunk(int x, int y, Chunk* chunk);
		void removeChunk(int x, int y);

	private:
		std::unordered_map<glm::ivec2, Chunk*> chunk_map;
		Shader* shader;
	};
}


#endif