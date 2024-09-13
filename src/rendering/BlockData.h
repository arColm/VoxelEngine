
#ifndef BLOCKDATA_H
#define BLOCKDATA_H
#include <glm/ext/vector_float4.hpp>

namespace VoxelEngine {
	enum class BlockType {
		Air,
		Dirt,
		Sand,
		Water,
		Snow,
		Grass,
		Wood,
		Leaves
	};
	namespace BlockData {
		//short id;
		//glm::vec3 color;

		//BlockData getBlockData(BlockType block);
		BlockType getBlockType(short id);
		glm::vec4 getBlockColor(BlockType block);
		bool getBlockTransparent(BlockType block);
	};

}

#endif