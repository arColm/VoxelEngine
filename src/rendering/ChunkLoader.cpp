

#include "ChunkLoader.h"
#include <Camera.h>

namespace VoxelEngine{
	ChunkLoader::ChunkLoader() {
		ChunkLoader::shader = std::make_shared<Shader>("src/shaders/vertexShader.glsl", "src/shaders/fragmentShader.glsl");
		updateProjectionMatrix(shader->projectionLoc);
	}
	ChunkLoader::ChunkLoader(std::shared_ptr<Shader> shader) {
		ChunkLoader::shader = shader;
	}
	ChunkLoader::~ChunkLoader() { }

	void ChunkLoader::loadChunks() {
		
		for(auto& pair : chunk_map) {
			pair.second->load();
		}

	}
	void ChunkLoader::renderChunks() {
		shader->use();
		updateProjectionMatrix(shader->projectionLoc);
		setViewMatrix(shader->viewLoc);
		for (auto& pair : chunk_map) {
			pair.second->render(shader);
		}
	}

	void ChunkLoader::addChunk(int x, int y, std::shared_ptr<Chunk> chunk) {
		ChunkLoader::chunk_map.insert_or_assign(glm::ivec2(x, y), chunk);
	}
	void ChunkLoader::removeChunk(int x, int y) {

	}
}