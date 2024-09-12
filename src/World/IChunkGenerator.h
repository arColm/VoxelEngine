
#ifndef ICHUNKGENERATOR_H
#define ICHUNKGENERATOR_H

#include <src/rendering/Chunk.h>

namespace VoxelEngine {
	class IChunkGenerator {
	public:
		virtual std::shared_ptr<Chunk> generateChunk(int x, int z);
		virtual void generateTerrain(const std::shared_ptr<Chunk>& chunk);
		virtual void decorateChunk(const std::shared_ptr<Chunk>& chunk, const std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>& chunk_map);
		virtual ~IChunkGenerator();
		
		virtual void generateGUI();
	};
}


#endif