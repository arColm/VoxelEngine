

#include "ChunkLoader.h"

namespace VoxelEngine{

	ChunkLoader::ChunkLoader(Shader* shader) {
		ChunkLoader::shader = shader;
	}
	ChunkLoader::~ChunkLoader() { }

	void ChunkLoader::loadChunks() {
		
		for(auto pair : chunk_map) {
			Chunk* chunk = pair.second;
			chunk->load(pair.first.x, pair.first.y, shader);
		}

	}
	void ChunkLoader::renderChunks() {
		for (auto pair : chunk_map) {
			Chunk* chunk = pair.second;
			chunk->render();
		}
	}

	void ChunkLoader::addChunk(int x, int y, Chunk* chunk) {
		chunk_map[glm::ivec2(x,y)] = chunk;
	}
	void ChunkLoader::removeChunk(int x, int y) {

	}
}