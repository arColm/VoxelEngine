
#include "src/World/FlatChunkGenerator.h"


namespace VoxelEngine {
	Chunk* FlatChunkGenerator::generateChunk(int x, int z) {
		Chunk chunk = Chunk(x, z);
		for (int x = 0; x < 16; x += 2) {
			for (int z = 0; z < 16; z += 2) {
				chunk.setBlock(x, 3, z, BlockType::Dirt);
			}
		}
		return &chunk;
	}
}