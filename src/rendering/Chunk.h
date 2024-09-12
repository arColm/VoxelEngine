#ifndef CHUNK_H
#define CHUNK_H


#include <src/rendering/block.h>
#include <src/shaders/shader.h>
#include "BlockData.h"
#include <vector>
//#include <glm/glm.hpp>
#include <unordered_map>
#include <array>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

namespace VoxelEngine {
	class Chunk {
	public:
		static enum State {
			Empty,
			TerrainGenerated,
			WaitingForMesh,
			MeshGenerated,
			Unloaded
		};
		Chunk(int x, int z);
		~Chunk();

		static const int HEIGHT = 64,
			WIDTH = 32;

		void load(std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>* chunk_map);
		void renderOpaque();
		void renderTransparent();
		void renderWater();
		void addBlock(float x, float y,float z, BlockType block, std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>* chunk_map);
		void setBlock(float x, float y, float z, BlockType block);

		void generateMesh();

		BlockType getBlock(int x, int y, int z);
		int getX();
		int getZ();
		bool getHasMesh();

		void updateState(State newState);
		State getState();
	private:
		int x, z;
		GLuint opaqueVAO;
		GLuint transparentVAO;
		GLuint waterVAO;

		int numOpaqueVertices;
		int numTransparentVertices;
		int numWaterVertices;
		bool hasMesh;
		State state;

		std::array<std::array<std::array<VoxelEngine::BlockType,WIDTH>,HEIGHT>,WIDTH> blocks;

		void addTop(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addLeft(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addRight(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addFront(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addBack(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addBottom(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		

		std::vector<GLfloat> opaqueVertexPos;
		std::vector<GLfloat> opaqueVertexColor;
		std::vector<GLfloat> opaqueVertexNormals;

		std::vector<GLfloat> transparentVertexPos;
		std::vector<GLfloat> transparentVertexColor;
		std::vector<GLfloat> transparentVertexNormals;

		std::vector<GLfloat> waterVertexPos;
		std::vector<GLfloat> waterVertexColor;
		std::vector<GLfloat> waterVertexNormals;

		void createVAO();
	};


	
}


#endif