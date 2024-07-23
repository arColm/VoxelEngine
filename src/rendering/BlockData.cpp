#include "BlockData.h"



namespace VoxelEngine {


	BlockType BlockData::getBlockType(short id) {
		return static_cast<BlockType>(id);
	}
	glm::vec4 BlockData::getBlockColor(BlockType block) {
		switch (block) {
		case BlockType::Air:
			return glm::vec4(0.f, 0.f, 0.f, 1.0f);
		case BlockType::Dirt:
			return glm::vec4(0.34f, 0.17f, 0.04f,1.0f);
		case BlockType::Sand:
			return glm::vec4(0.79f, 0.79f, 0.05f, 1.0f);
		case BlockType::Water:
			return glm::vec4(0.30f, 0.55f, 0.92f, 0.5f);
		}
	}
}