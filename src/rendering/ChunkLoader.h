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
		void renderMeshes();
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Shader> lightShader;
		unsigned int depthMapFBO;
		unsigned int depthMap;

		//unsigned int quadVAO;
		//unsigned int quadVBO;
		//std::shared_ptr<Shader> debugDepthShader;

		//void renderQuad();

	};
}


#endif