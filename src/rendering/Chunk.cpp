
#include <src/rendering/chunk.h>
#include <vector>
#include <src/rendering/loader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace VoxelEngine {
	Chunk::Chunk(int x, int y) {
		Chunk::x = x;
		Chunk::y = y;

		//initialize / load chunk here
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				for (int z = 0; z < WIDTH; z++) {
					blocks[x][y][z] = BlockType::Air;
				}
			}
		}
	}
	Chunk::~Chunk() {
	}
	void Chunk::render(int chunkX, int chunkZ, Shader* shader)
	{
		shader->use();
		GLuint VAO = Loader::createVAO();
		glBindVertexArray(VAO);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(WIDTH * chunkX, 0.f, WIDTH * chunkZ));
		unsigned int modelLoc = glGetUniformLocation(shader->ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				for (int z = 0; z < WIDTH; z++) {
					if (blocks[x][y][z] == BlockType::Air) continue;
					addBlock(x, y, z, blocks[x][y][z]);
				}
			}
		}
		Chunk::drawBlocks(VAO);
	}

	void Chunk::setBlock(float x, float y, float z, BlockType block) {
		Chunk::blocks[(int)x][(int)y][(int)z] = block;
	}

	void Chunk::addBlock(float x,float y,float z, BlockType block) {
		Chunk::addTop(x, y, z, block);

	}

	void Chunk::addTop(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::vector<GLfloat> newVertices = {
			x, y + 1, z,
			x, y + 1, z + 1,
			x + 1,y + 1,z + 1,
			x + 1,y + 1,z + 1,
			x + 1,y + 1,z,
			x,y + 1,z
		};
		std::vector<GLfloat> newColors = {
			0.34f,0.17f,0.04f,
			0.34f,0.17f,0.04f,
			0.34f,0.17f,0.04f,
			0.34f,0.17f,0.04f,
			0.34f,0.17f,0.04f,
			0.34f,0.17f,0.04f
		};

		vertexPos.insert(vertexPos.end(), newVertices.begin(), newVertices.end());
		vertexColor.insert(vertexColor.end(), newColors.begin(), newColors.end());
	}
	void Chunk::addLeft(GLfloat x, GLfloat y, GLfloat z, BlockType block) {

	}
	void Chunk::addRight(GLfloat x, GLfloat y, GLfloat z, BlockType block) {

	}
	void Chunk::addForward(GLfloat x, GLfloat y, GLfloat z, BlockType block) {

	}
	void Chunk::addBack(GLfloat x, GLfloat y, GLfloat z, BlockType block) {

	}
	void Chunk::addBottom(GLfloat x, GLfloat y, GLfloat z, BlockType block) {

	}
	void Chunk::drawBlocks(GLuint VAO) {

		// position attribute
		GLuint positionVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER,positionVBO);
		glBufferData(GL_ARRAY_BUFFER, vertexPos.size() * sizeof(vertexPos.at(0)), vertexPos.data(), GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPos), vertexPos.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		// color attribute
		GLuint colorVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, vertexColor.size()*sizeof(vertexColor.at(0)), vertexColor.data(), GL_STATIC_DRAW);


		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);




		glDrawArrays(GL_TRIANGLES, 0, vertexPos.size()/3);

		//std::cout << vertexPos.data()[0] << '-' << vertexPos.data()[1] << '-' << vertexPos.data()[2] << std::endl;
		//std::cout << vertexPos[3] << '-' << vertexPos[4] << '-' << vertexPos[5] << std::endl;
		//std::cout << vertexPos[6] << '-' << vertexPos[7] << '-' << vertexPos[8] << std::endl;
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &positionVBO);
		glDeleteBuffers(1, &colorVBO);
		vertexPos.clear();
		vertexColor.clear();
	}



}

