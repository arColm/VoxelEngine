
#include "src/World/IslandChunkGenerator.h"
#include <src/Utils/Hashing/xxHash.h>
#include <src/Utils/Noise/PerlinNoise.h>


namespace VoxelEngine {
	std::shared_ptr<Chunk> IslandChunkGenerator::generateChunk(int x, int z) {
		std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(x, z);
		int WIDTH = Chunk::getWidth();

		std::vector<std::vector<float>> heightMap = PerlinNoise::GetLattice2D(x,z,WIDTH,3,2.0f,0.5f);

		for (int xi = 0; xi < WIDTH; xi++) {
			for (int zi = 0; zi < WIDTH; zi++) {
				int yi = 40 * heightMap[xi][zi];

				if (yi >= 15) {
					if (yi >= 30) {
						for (int i = 0; i < 3; i++, yi--) {
							chunk->setBlock(xi, yi, zi, BlockType::Snow);
						}
					}
					else {
						for (int i = 0; i < 3; i++, yi--) {
							chunk->setBlock(xi, yi, zi, BlockType::Grass);
						}
					}

					for (; yi >= 15; yi--) {
						chunk->setBlock(xi, yi, zi, BlockType::Dirt);
					}
				}
				else {
					for (int i = 14; i > yi; i--) {
						chunk->setBlock(xi, i, zi, BlockType::Water);
					}
				}
				for (; yi >= 0; yi--) {
					chunk->setBlock(xi, yi, zi, BlockType::Sand);
				}
			}
		}
		return chunk;

	}
}