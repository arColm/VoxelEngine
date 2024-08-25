#include "src/World/PerlinChunkGenerator.h"
#include <src/Utils/Hashing/xxHash.h>
#include <src/Utils/Noise/PerlinNoise.h>
#include <imgui.h>



namespace VoxelEngine {

	float PerlinChunkGenerator::persistence = 0.875f;
	float PerlinChunkGenerator::lacunarity = 1.143f;
	int PerlinChunkGenerator::numOctaves = 5;
	std::shared_ptr<Chunk> PerlinChunkGenerator::generateChunk(int x, int z) {
		std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(x, z);
		int WIDTH = Chunk::WIDTH;

		std::vector<std::vector<float>> heightMap = PerlinNoise::GetLattice2D(x, z, WIDTH, numOctaves, lacunarity, persistence);

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

	void PerlinChunkGenerator::generateGUI() {

		ImGui::SliderFloat("Persistence", &persistence, 0.3f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		ImGui::SliderFloat("Lacunarity", &lacunarity, 1.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		ImGui::SliderInt("numOctaves", &numOctaves, 1, 7);            // Edit 1 float using a slider from 0.0f to 1.0f
	}
}