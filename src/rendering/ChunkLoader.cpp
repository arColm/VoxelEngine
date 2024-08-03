

#include "ChunkLoader.h"
#include <Camera.h>
#include <thread>

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
		std::vector < std::thread> threads;

		for(const auto& pair : chunk_map) {
			threads.push_back(std::thread(&Chunk::load, pair.second, &chunk_map));
			//pair.second->load(&chunk_map);
		}
		for (auto& t : threads) {
			t.join();
		}
		/*for (const auto& pair : chunk_map) {
			pair.second->loadBlocks();
		}*/
	}
	void ChunkLoader::renderChunks() {
		shader->use();
		updateProjectionMatrix(shader->projectionLoc);
		setViewMatrix(shader->viewLoc);
		for (const auto& pair : chunk_map) {
			pair.second->renderOpaque(shader);
		}
		for (const auto& pair : chunk_map) {
			pair.second->renderTransparent(shader);
		}
	}

	void ChunkLoader::addChunk(int x, int y, std::shared_ptr<Chunk> chunk) {
		ChunkLoader::chunk_map.insert_or_assign(glm::ivec2(x, y), chunk);
	}
	void ChunkLoader::removeChunk(int x, int y) {

	}
}