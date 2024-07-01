

#include "ChunkLoader.h"
#include <Camera.h>

namespace VoxelEngine{

	ChunkLoader::ChunkLoader(Shader* shader) {
		ChunkLoader::shader = shader;
	}
	ChunkLoader::~ChunkLoader() { }

	void ChunkLoader::loadChunks() {
		
		for(auto& pair : chunk_map) {
			pair.second->load();
		}

	}
	void ChunkLoader::renderChunks() {
		Camera::setViewMatrix(shader->viewLoc);
		for (auto& pair : chunk_map) {
			pair.second->render(shader);
		}
	}

	void ChunkLoader::addChunk(int x, int y, Chunk* chunk) {
		chunk_map[glm::ivec2(x,y)] = chunk;
	}
	void ChunkLoader::removeChunk(int x, int y) {

	}
}