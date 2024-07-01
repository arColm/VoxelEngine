#ifndef CHUNK_H
#define CHUNK_H


#include <src/rendering/block.h>
#include <src/shaders/shader.h>
#include "BlockData.h"
#include <vector>
//#include <glm/glm.hpp>

namespace VoxelEngine {
	class Chunk {
	public:
		Chunk(int x, int z);
		~Chunk();

		void load();
		void render(Shader* shader);
		void addBlock(float x, float y,float z, BlockType block);
		void setBlock(float x, float y, float z, BlockType block);

	private:
		int x, z;
		static const int HEIGHT = 64,
			WIDTH = 16;
		GLuint VAO;
		int numVertices;
		//VoxelEngine::BlockType blocks[WIDTH][HEIGHT][WIDTH];
		std::vector<std::vector<std::vector<VoxelEngine::BlockType>>> blocks;

		void addTop(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addLeft(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addRight(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addForward(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addBack(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addBottom(GLfloat x, GLfloat y, GLfloat z, BlockType block);

		void loadBlocks();

		std::vector<GLfloat> vertexPos;
		std::vector<GLfloat> vertexColor;
	};


	
}


#endif