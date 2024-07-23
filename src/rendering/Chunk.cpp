
#include <src/rendering/chunk.h>
#include <vector>
#include <src/rendering/loader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <src/Utils/Hashing/xxHash.h>

namespace VoxelEngine {
	Chunk::Chunk(int x, int z) { //probably use Generator as parameter
		Chunk::x = x;
		Chunk::z = z;
		Chunk::numVertices = 0;
		Chunk::VAO = Loader::createVAO();

		//initialize / load chunk here
		blocks = std::vector<std::vector<std::vector<VoxelEngine::BlockType>>>
			(WIDTH, std::vector<std::vector<VoxelEngine::BlockType>>(HEIGHT, std::vector<VoxelEngine::BlockType>(WIDTH,BlockType::Air)));
	}
	Chunk::~Chunk() {
		glDeleteVertexArrays(1, &VAO);
	}
	void Chunk::load()
	{
		glBindVertexArray(VAO);

		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				for (int z = 0; z < WIDTH; z++) {
					if (blocks[x][y][z] == BlockType::Air) continue;
					addBlock(x, y, z, blocks[x][y][z]);
				}
			}
		}
		Chunk::loadBlocks();
	}

	void Chunk::loadBlocks() {

		// position attribute
		GLuint positionVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glBufferData(GL_ARRAY_BUFFER, vertexPos.size() * sizeof(vertexPos.at(0)), vertexPos.data(), GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPos), vertexPos.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		// color attribute
		GLuint colorVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(vertexColor.at(0)), vertexColor.data(), GL_STATIC_DRAW);


		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);


		//std::cout << vertexPos.data()[0] << '-' << vertexPos.data()[1] << '-' << vertexPos.data()[2] << std::endl;
		//std::cout << vertexPos[3] << '-' << vertexPos[4] << '-' << vertexPos[5] << std::endl;
		//std::cout << vertexPos[6] << '-' << vertexPos[7] << '-' << vertexPos[8] << std::endl;
		glBindVertexArray(0);

		glDeleteBuffers(1, &positionVBO);
		glDeleteBuffers(1, &colorVBO);
		Chunk::numVertices = vertexPos.size() / 3;
		vertexPos.clear();
		vertexColor.clear();
	}

	void Chunk::render(std::shared_ptr<Shader> shader) {

		shader->use();
		glBindVertexArray(VAO);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(WIDTH * x, 0.f, WIDTH * z));
		glUniformMatrix4fv(shader->modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, numVertices);
		glBindVertexArray(0);
	}

	void Chunk::setBlock(float x, float y, float z, BlockType block) {
		Chunk::blocks[(int)x][(int)y][(int)z] = block;
	}

	void Chunk::addBlock(float x,float y,float z, BlockType block) {
		if (y == HEIGHT - 1 || blocks[x][y + 1][z] == BlockType::Air) {
			Chunk::addTop(x, y, z, block);
		}
		if (y == 0 || blocks[x][y - 1][z] == BlockType::Air) {
			Chunk::addBottom(x, y, z, block);
		}
		if (x == WIDTH - 1 || blocks[x + 1][y][z] == BlockType::Air) {
			Chunk::addRight(x, y, z, block);
		}
		if (x == 0 || blocks[x - 1][y][z] == BlockType::Air) {
			Chunk::addLeft(x, y, z, block);
		}
		if (z == WIDTH - 1 || blocks[x][y][z + 1] == BlockType::Air) {
			Chunk::addBack(x, y, z, block);
		}
		if (z == 0 || blocks[x][y][z - 1] == BlockType::Air) {
			Chunk::addForward(x, y, z, block);
		}

	}

	glm::vec4 randomizeColor(glm::vec4 color, glm::vec3 pos) {

		xxHash hash(0);
		hash = hash.eat(pos.y);
		float var = (uint32_t)hash / static_cast<float>(std::numeric_limits<uint32_t>::max());
		return glm::vec4(color.x * var, color.y * var, color.z * var,color.w);
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
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::vector<GLfloat> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};

		vertexPos.insert(vertexPos.end(), newVertices.begin(), newVertices.end());
		vertexColor.insert(vertexColor.end(), newColors.begin(), newColors.end());
	}
	void Chunk::addLeft(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::vector<GLfloat> newVertices = {
			x, y + 1, z,
			x, y + 1, z + 1,
			x,y,z + 1,
			x,y,z + 1,
			x,y,z,
			x,y + 1,z
		};
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::vector<GLfloat> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};

		vertexPos.insert(vertexPos.end(), newVertices.begin(), newVertices.end());
		vertexColor.insert(vertexColor.end(), newColors.begin(), newColors.end());
	}
	void Chunk::addRight(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::vector<GLfloat> newVertices = {
			x+1, y + 1, z,
			x + 1, y + 1, z + 1,
			x + 1,y,z + 1,
			x + 1,y,z + 1,
			x + 1,y,z,
			x + 1,y + 1,z
		};
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::vector<GLfloat> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};

		vertexPos.insert(vertexPos.end(), newVertices.begin(), newVertices.end());
		vertexColor.insert(vertexColor.end(), newColors.begin(), newColors.end());
	}
	void Chunk::addForward(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::vector<GLfloat> newVertices = {
			x, y + 1, z,
			x + 1, y + 1, z,
			x + 1,y,z,
			x + 1,y,z,
			x,y,z,
			x,y + 1,z
		};
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::vector<GLfloat> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};

		vertexPos.insert(vertexPos.end(), newVertices.begin(), newVertices.end());
		vertexColor.insert(vertexColor.end(), newColors.begin(), newColors.end());
	}
	void Chunk::addBack(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::vector<GLfloat> newVertices = {
			x, y + 1, z + 1,
			x + 1, y + 1, z + 1,
			x + 1,y,z + 1,
			x + 1,y,z + 1,
			x,y,z + 1,
			x,y + 1,z+1
		};
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::vector<GLfloat> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};

		vertexPos.insert(vertexPos.end(), newVertices.begin(), newVertices.end());
		vertexColor.insert(vertexColor.end(), newColors.begin(), newColors.end());
	}
	void Chunk::addBottom(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::vector<GLfloat> newVertices = {
			x, y , z,
			x, y , z + 1,
			x + 1,y ,z + 1,
			x + 1,y ,z + 1,
			x + 1,y,z,
			x,y ,z
		};
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::vector<GLfloat> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};

		vertexPos.insert(vertexPos.end(), newVertices.begin(), newVertices.end());
		vertexColor.insert(vertexColor.end(), newColors.begin(), newColors.end());
	}

	int Chunk::getWidth() {
		return WIDTH;
	}

}

