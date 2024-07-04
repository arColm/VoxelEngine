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
		ChunkLoader();
		ChunkLoader(Shader* shader);
		~ChunkLoader();
		void renderChunks();
		void loadChunks();
		void addChunk(int x, int y, Chunk* chunk);
		void removeChunk(int x, int y);


		std::unordered_map<glm::ivec2, Chunk*> chunk_map;
		std::unordered_map<int, Chunk*> chun_map;
	private:
		Shader* shader;
	};
}


#endif