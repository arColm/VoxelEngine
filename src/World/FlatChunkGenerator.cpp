
#include "src/World/FlatChunkGenerator.h"


namespace VoxelEngine {
	std::shared_ptr<Chunk> FlatChunkGenerator::generateChunk(int x, int z) {
		std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(x, z);
		for (int x = 0; x < 16; x ++) {
			for (int z = 0; z < 16; z ++) {
				chunk->setBlock(x, 3, z, BlockType::Dirt);
			}
		}
		return chunk;
	}
}