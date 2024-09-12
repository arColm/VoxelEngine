#include "src/World/PerlinChunkGenerator.h"
#include <src/Utils/Hashing/xxHash.h>
#include <src/Utils/Noise/PerlinNoise.h>
#include <imgui.h>



namespace VoxelEngine {

	float PerlinChunkGenerator::persistence = 0.875f;
	float PerlinChunkGenerator::lacunarity = 1.143f;
	int PerlinChunkGenerator::numOctaves = 5;
	int PerlinChunkGenerator::seaLevel = 15;
	int PerlinChunkGenerator::snowLevel = 30;
	int PerlinChunkGenerator::maxHeight = 40;
	std::shared_ptr<Chunk> PerlinChunkGenerator::generateChunk(int x, int z) {
		std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(x, z);

		return chunk;

	}

	void PerlinChunkGenerator::generateTerrain(const std::shared_ptr<Chunk>& chunk) {
		int WIDTH = Chunk::WIDTH;
		int x = chunk->getX();
		int z = chunk->getZ();
		std::vector<std::vector<float>> heightMap = PerlinNoise::GetLattice2D(x, z, WIDTH, numOctaves, lacunarity, persistence);

		for (int xi = 0; xi < WIDTH; xi++) {
			for (int zi = 0; zi < WIDTH; zi++) {
				int yi = maxHeight * heightMap[xi][zi];

				if (yi >= seaLevel) {
					if (yi >= snowLevel) {
						for (int i = 0; i < 3; i++, yi--) {
							chunk->setBlock(xi, yi, zi, BlockType::Snow);
						}
					}
					else {
						for (int i = 0; i < 3; i++, yi--) {
							chunk->setBlock(xi, yi, zi, BlockType::Grass);
						}
					}

					for (; yi >= seaLevel; yi--) {
						chunk->setBlock(xi, yi, zi, BlockType::Dirt);
					}
				}
				else {
					for (int i = seaLevel - 1; i > yi; i--) {
						chunk->setBlock(xi, i, zi, BlockType::Water);
					}
				}
				for (; yi >= 0; yi--) {
					chunk->setBlock(xi, yi, zi, BlockType::Sand);
				}
			}
		}
	}
	void PerlinChunkGenerator::decorateChunk(const std::shared_ptr<Chunk>& chunk, const std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunk_map) {

	}

	void PerlinChunkGenerator::generateGUI() {

		ImGui::SliderInt("Max Height", &maxHeight, 5, Chunk::HEIGHT);
		ImGui::SliderInt("Snow Level", &snowLevel, std::max(seaLevel, 4), maxHeight);
		ImGui::SliderInt("Sea Level", &seaLevel, 1, std::min(snowLevel, 30));
		ImGui::Separator();

		ImGui::SliderFloat("Persistence", &persistence, 0.3f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		ImGui::SliderFloat("Lacunarity", &lacunarity, 1.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		ImGui::SliderInt("numOctaves", &numOctaves, 1, 7);            // Edit 1 float using a slider from 0.0f to 1.0f

	}

}