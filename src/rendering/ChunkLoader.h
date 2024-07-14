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
		ChunkLoader(std::shared_ptr<Shader> shader);
		~ChunkLoader();
		void renderChunks();
		void loadChunks();
		void addChunk(int x, int y, std::shared_ptr<Chunk> chunk);
		void removeChunk(int x, int y);


		std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>> chunk_map;
	private:
		std::shared_ptr<Shader> shader;
	};
}


#endif