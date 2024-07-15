
#ifndef BLOCKDATA_H
#define BLOCKDATA_H
#include <glm/ext/vector_float3.hpp>

namespace VoxelEngine {
	enum class BlockType {
		Air,
		Dirt,
		Sand
	};
	namespace BlockData {
		//short id;
		//glm::vec3 color;

		//BlockData getBlockData(BlockType block);
		BlockType getBlockType(short id);
		glm::vec3 getBlockColor(BlockType block);
	};

}

#endif