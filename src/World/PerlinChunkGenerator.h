#pragma once
#include <src/World/IChunkGenerator.h>


namespace VoxelEngine {
	class PerlinChunkGenerator : public VoxelEngine::IChunkGenerator {
	public:
		static float persistence;
		static float lacunarity;
		static int numOctaves;
		static int seaLevel;
		static int snowLevel;
		static int maxHeight;
		std::shared_ptr<Chunk> generateChunk(int x, int z);
		void generateTerrain(const std::shared_ptr<Chunk>& chunk);
		void decorateChunk(const std::shared_ptr<Chunk>& chunk, const std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunk_map);

		void generateGUI();
	};
}
