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
		Chunk(int x, int z);
		~Chunk();

		void load(std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>* chunk_map);
		void renderOpaque(std::shared_ptr<Shader> shader);
		void renderTransparent(std::shared_ptr<Shader> shader);
		//void render(std::shared_ptr<Shader> shader);
		void addBlock(float x, float y,float z, BlockType block, std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>* chunk_map);
		void setBlock(float x, float y, float z, BlockType block);
		BlockType getBlock(int x, int y, int z);
		static int getWidth();
		int getX();
		int getZ();

		void loadBlocks();

	private:
		int x, z;
		static const int HEIGHT = 64,
			WIDTH = 18;
		GLuint opaqueVAO;
		GLuint transparentVAO;
		int numOpaqueVertices;
		int numTransparentVertices;
		bool hasValidChunkData;
		//VoxelEngine::BlockType blocks[WIDTH][HEIGHT][WIDTH];
		std::array<std::array<std::array<VoxelEngine::BlockType,WIDTH>,HEIGHT>,WIDTH> blocks;

		void addTop(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addLeft(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addRight(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addForward(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addBack(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addBottom(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		

		std::vector<GLfloat> opaqueVertexPos;
		std::vector<GLfloat> opaqueVertexColor;

		std::vector<GLfloat> transparentVertexPos;
		std::vector<GLfloat> transparentVertexColor;

		void createVAO();
	};


	
}


#endif