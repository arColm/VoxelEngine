
#include "src/World/IslandChunkGenerator.h"
#include <src/Utils/Hashing/xxHash.h>


namespace VoxelEngine {
	std::shared_ptr<Chunk> IslandChunkGenerator::generateChunk(int x, int z) {
		std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(x, z);
		xxHash x0, x1;
		xxHash h00, h01, h10, h11;
		x0 = x0.eat(x);
		x1 = x1.eat(x + 1);
		h00 = x0.eat(z);
		h01 = x0.eat(z + 1);
		h10 = x1.eat(z);
		h11 = x1.eat(z + 1);

		float p00, p01, p10, p11;
		p00 = h00 / static_cast<float>(std::numeric_limits<uint32_t>::max());
		p01 = h01 / static_cast<float>(std::numeric_limits<uint32_t>::max());
		p10 = h10 / static_cast<float>(std::numeric_limits<uint32_t>::max());
		p11 = h11 / static_cast<float>(std::numeric_limits<uint32_t>::max());


		for (int xi = 0; xi < 16; xi++) {
			for (int zi = 0; zi < 16; zi++) {
				float u = xi / 15.f;
				float v = zi / 15.f;
				int yi = 20 * std::lerp(std::lerp(p00, p10, u), std::lerp(p01, p11, u), v);
				if (yi >= 15) {
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