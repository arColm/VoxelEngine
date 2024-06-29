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
		Chunk(int x, int y);
		~Chunk();

		void render(int chunkX, int chunkZ, Shader* shader);
		void addBlock(float x, float y,float z, BlockType block);
		void setBlock(float x, float y, float z, BlockType block);

	private:
		int x, y;
		static const int HEIGHT = 64,
			WIDTH = 16;
		GLuint VAO;
		VoxelEngine::BlockType blocks[WIDTH][HEIGHT][WIDTH];

		void addTop(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addLeft(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addRight(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addForward(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addBack(GLfloat x, GLfloat y, GLfloat z, BlockType block);
		void addBottom(GLfloat x, GLfloat y, GLfloat z, BlockType block);

		void drawBlocks();

		std::vector<GLfloat> vertexPos;
		std::vector<GLfloat> vertexColor;
	};


	
}


#endif