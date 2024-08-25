#pragma once
#include <src/World/IChunkGenerator.h>


namespace VoxelEngine {
	class PerlinChunkGenerator : public VoxelEngine::IChunkGenerator {
	public:
		static float persistence;
		static float lacunarity;
		static int numOctaves;
		std::shared_ptr<Chunk> generateChunk(int x, int z);

		void generateGUI();
	};
}
