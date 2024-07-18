#include "src/World/PyramidChunkGenerator.h"


namespace VoxelEngine {
	std::shared_ptr<Chunk> PyramidChunkGenerator::generateChunk(int x, int z) {
		std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(x, z);
		for (int x = 0; x < 16; x++) {
			for (int z = 0; z < 16; z++) {
				chunk->setBlock(x, std::min(std::abs(8-x), std::abs(8 - z)), z, BlockType::Dirt);
			}
		}
		return chunk;
	}
}