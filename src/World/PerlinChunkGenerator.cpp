#include "src/World/PerlinChunkGenerator.h"
#include <src/Utils/Hashing/xxHash.h>



namespace VoxelEngine {
	std::shared_ptr<Chunk> PerlinChunkGenerator::generateChunk(int x, int z) {
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
		p00 = (h00 / static_cast<float>(std::numeric_limits<uint32_t>::max())) * 10;
		p01 = (h01 / static_cast<float>(std::numeric_limits<uint32_t>::max())) * 10;
		p10 = (h10 / static_cast<float>(std::numeric_limits<uint32_t>::max())) * 10;
		p11 = (h11 / static_cast<float>(std::numeric_limits<uint32_t>::max())) * 10;

		
		for (int xi = 0; xi < Chunk::getWidth(); xi++) {
			for (int zi = 0; zi < Chunk::getWidth(); zi++) {
				float u = xi / (Chunk::getWidth()-1.f);
				float v = zi / (Chunk::getWidth() - 1.f);
				int yi = std::lerp(std::lerp(p00, p10, u), std::lerp(p01, p11, u), v);
				for (; yi >= 0; yi--) {
					chunk->setBlock(xi, yi, zi, BlockType::Dirt);
				}
			}
		}
		return chunk;
	}
}