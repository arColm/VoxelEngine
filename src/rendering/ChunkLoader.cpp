

#include "ChunkLoader.h"
#include <Camera.h>

namespace VoxelEngine{
	ChunkLoader::ChunkLoader() {
		Shader s("src/shaders/vertexShader.glsl", "src/shaders/fragmentShader.glsl");
		s.use();
		ChunkLoader::shader = &s;
	}
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
		mainCamera->setViewMatrix(shader->viewLoc);
		for (auto& pair : chunk_map) {
			pair.second->render(shader);
		}
	}

	void ChunkLoader::addChunk(int x, int y, std::shared_ptr<Chunk> chunk) {
		//chunk_map[glm::ivec2(x,y)] = chunk;
		ChunkLoader::chunk_map.insert_or_assign(glm::ivec2(x, y), chunk);
		//ChunkLoader::chun_map.insert_or_assign(x, chunk);
	}
	void ChunkLoader::removeChunk(int x, int y) {

	}
}