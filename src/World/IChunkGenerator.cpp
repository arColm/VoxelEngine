

#include "src/World/IChunkGenerator.h"


namespace VoxelEngine {
	std::shared_ptr<Chunk> IChunkGenerator::generateChunk(int x, int z) { return NULL; }
	void IChunkGenerator::generateTerrain(const std::shared_ptr<Chunk>& chunk) { };
	void IChunkGenerator::decorateChunk(const std::shared_ptr<Chunk>& chunk, const std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunk_map) { };
	IChunkGenerator::~IChunkGenerator() { }

	void IChunkGenerator::generateGUI() { }

}