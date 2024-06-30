
#ifndef WORLD_H
#define WORLD_H

#include "IChunkGenerator.h"
namespace VoxelEngine {

	class World {
		int seed;
		IChunkGenerator* generator;
	};
}


#endif