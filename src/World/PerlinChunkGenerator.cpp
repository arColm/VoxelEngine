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
	float PerlinChunkGenerator::treeDensity = 0.1f;
	std::shared_ptr<Chunk> PerlinChunkGenerator::generateChunk(int x, int z) {
		std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(x, z);

		return chunk;

	}

	std::vector<std::vector<float>> generateTreeMap(int x, int z, int width, float treeDensity) {
		std::vector<std::vector<float>> res(width, std::vector<float>(width, 0));
		if (treeDensity == 0) return res;
		int numCells = width * treeDensity;
		int cellWidth = width / numCells;

		for (int cellX = 0; cellX < numCells; cellX++) {
			xxHash xHash;
			xHash = xHash.eat(numCells*x+cellX);
			for (int cellZ = 0; cellZ < numCells; cellZ++) {
				xxHash xzHash = xHash.eat(numCells * z + cellZ);
				//determine if cell will have a tree
				if (xzHash.Floats01A() > treeDensity) {
					continue;
				}
				//if cell has a tree, determine where in the cell the tree is
				int treeX = xzHash.Floats01B() * cellWidth;
				int treeZ = xzHash.Floats01C() * cellWidth;

				res[cellX*cellWidth+treeX][cellZ*cellWidth+treeZ] = 1;


			}
		}
		return res;
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

	void placeBlockUnbound(const std::shared_ptr<Chunk>& chunk, const std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunk_map,
		int x, int y, int z, BlockType block) {
		if (x >= 0 && x < Chunk::WIDTH && z >= 0 && z < Chunk::WIDTH) {
			chunk->setBlock(x, y, z, block, false);
		}
		else {
			int chunkX = chunk->getX();
			int chunkZ = chunk->getZ();

			if (x < 0) {
				chunkX--;
				x = Chunk::WIDTH + x;
			}
			else if (x >= Chunk::WIDTH) {
				chunkX++;
				x = x - Chunk::WIDTH;
			}
			
			if (z < 0) {
				chunkZ--;
				z = Chunk::WIDTH + z;
			}
			else if (z >= Chunk::WIDTH) {
				chunkZ++;
				z = z - Chunk::WIDTH;
			}

			chunk_map.find(glm::ivec2(chunkX,chunkZ))->second->setBlock(x,y,z,block,false);
		}


	}
	void placeTree(const std::shared_ptr<Chunk>& chunk, const std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunk_map, int x, int y, int z) {
		int chunkX = chunk->getX();
		int chunkZ = chunk->getZ();

		//TODO : THIS IS REALLY UGLY REFACTOR IT
		chunk->setBlock(x, y, z, BlockType::Wood, false);
		chunk->setBlock(x, y+1, z, BlockType::Wood, false);
		chunk->setBlock(x, y+2, z, BlockType::Wood, false);
		chunk->setBlock(x, y+3, z, BlockType::Wood, false);
		chunk->setBlock(x, y+4, z, BlockType::Wood, false);
		chunk->setBlock(x, y+5, z, BlockType::Wood, false);
		chunk->setBlock(x, y+6, z, BlockType::Wood, false);
		chunk->setBlock(x, y+7, z, BlockType::Leaves, false);
		chunk->setBlock(x, y+8, z, BlockType::Leaves, false);

		placeBlockUnbound(chunk, chunk_map, x+1, y+7, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y+7, z+1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x-1, y+7, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y+7, z-1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x + 1, y + 6, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 6, z + 1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x - 1, y + 6, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 6, z - 1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x + 1, y + 5, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 5, z + 1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x - 1, y + 5, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 5, z - 1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x + 1, y + 4, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 4, z + 1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x - 1, y + 4, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 4, z - 1, BlockType::Leaves);

		placeBlockUnbound(chunk, chunk_map, x + 1, y + 4, z+1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x+1, y + 4, z -1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x - 1, y + 4, z+1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x-1, y + 4, z - 1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x + 2, y + 4, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 4, z + 2, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x - 2, y + 4, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 4, z - 2, BlockType::Leaves);

		placeBlockUnbound(chunk, chunk_map, x + 1, y + 3, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 3, z + 1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x - 1, y + 3, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 3, z - 1, BlockType::Leaves);

		placeBlockUnbound(chunk, chunk_map, x + 1, y + 3, z + 1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x + 1, y + 3, z - 1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x - 1, y + 3, z + 1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x - 1, y + 3, z - 1, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x + 2, y + 3, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 3, z + 2, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x - 2, y + 3, z, BlockType::Leaves);
		placeBlockUnbound(chunk, chunk_map, x, y + 3, z - 2, BlockType::Leaves);

	}
	void PerlinChunkGenerator::decorateChunk(const std::shared_ptr<Chunk>& chunk, const std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunk_map) {

		int WIDTH = Chunk::WIDTH;
		int x = chunk->getX();
		int z = chunk->getZ();
		std::vector<std::vector<float>> treeMap = generateTreeMap(x, z, WIDTH, treeDensity);
		for (int xi = 0; xi < WIDTH; xi++) {
			for (int zi = 0; zi < WIDTH; zi++) {
				if (treeMap[xi][zi] > 0) {
					int treeBase = chunk->getGroundHeight(xi, zi);
					if (treeBase <= seaLevel || treeBase >= Chunk::HEIGHT - 7) continue;
					placeTree(chunk, chunk_map, xi, treeBase, zi);
					//chunk->setBlock(xi, treeBase, zi, BlockType::Wood, false);

				}

			}
		}
	}

	void PerlinChunkGenerator::generateGUI() {

		ImGui::SliderFloat("Tree Density", &treeDensity, 0.f, 1.f);
		ImGui::Separator();
		ImGui::SliderInt("Max Height", &maxHeight, 5, Chunk::HEIGHT);
		ImGui::SliderInt("Snow Level", &snowLevel, std::max(seaLevel, 4), maxHeight);
		ImGui::SliderInt("Sea Level", &seaLevel, 1, std::min(snowLevel, 30));
		ImGui::Separator();

		ImGui::SliderFloat("Persistence", &persistence, 0.3f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		ImGui::SliderFloat("Lacunarity", &lacunarity, 1.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		ImGui::SliderInt("numOctaves", &numOctaves, 1, 7);            // Edit 1 float using a slider from 0.0f to 1.0f

	}

}