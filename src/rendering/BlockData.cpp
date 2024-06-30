#include "BlockData.h"



namespace VoxelEngine {

	//BlockData BlockData::getBlockData(BlockType block) {
	//	switch (block) {
	//	case BlockType::Air:
	//		return { 0 , glm::vec3(0.f,0.f,0.f) };
	//	case BlockType::Dirt:
	//		return { 1 , glm::vec3(0.34f,0.17f,0.04f) };
	//	}
	//	
	//	return { -1, glm::vec3(0.f, 0.f, 0.f)};
	//}

	BlockType BlockData::getBlockType(short id) {
		return static_cast<BlockType>(id);
	}
	glm::vec3 BlockData::getBlockColor(BlockType block) {
		switch (block) {
		case BlockType::Air:
			return glm::vec3(0.f, 0.f, 0.f);
		case BlockType::Dirt:
			return glm::vec3(0.34f, 0.17f, 0.04f);
		}
	}
}