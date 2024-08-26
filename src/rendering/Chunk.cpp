
#include <src/rendering/chunk.h>
#include <vector>
#include <src/rendering/loader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <src/Utils/Hashing/xxHash.h>
#include <array>
#include <algorithm>

namespace VoxelEngine {
	Chunk::Chunk(int x, int z) {
		Chunk::x = x;
		Chunk::z = z;
		Chunk::numOpaqueVertices = 0;
		Chunk::numTransparentVertices = 0;
		Chunk::opaqueVAO = 0;
		Chunk::transparentVAO = 0;
		Chunk::hasMesh = false;


		//initialize / load chunk here
		//blocks = std::vector<std::vector<std::vector<VoxelEngine::BlockType>>>
		//	(WIDTH, std::vector<std::vector<VoxelEngine::BlockType>>(HEIGHT, std::vector<VoxelEngine::BlockType>(WIDTH,BlockType::Air)));
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				for (int z = 0; z < WIDTH; z++) {
					blocks[x][y][z] = BlockType::Air;
				}
			}
		}
	}
	Chunk::~Chunk() {
		glDeleteVertexArrays(1, &opaqueVAO);
		glDeleteVertexArrays(1, &transparentVAO);
	}
	void Chunk::load(std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>* chunk_map)
	{
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				for (int z = 0; z < WIDTH; z++) {
					if (blocks[x][y][z] == BlockType::Air) continue;
					addBlock(x, y, z, blocks[x][y][z],chunk_map);
				}
			}
		}
		hasMesh = true;
	}
	void Chunk::createVAO() {
		Chunk::opaqueVAO = Loader::createVAO();
		Chunk::transparentVAO = Loader::createVAO();

	}

	void Chunk::loadBlocks() {
		if (opaqueVAO == transparentVAO) createVAO();
		// opaque blocks
		// 
		glBindVertexArray(opaqueVAO);
		// position attribute
		GLuint positionVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glBufferData(GL_ARRAY_BUFFER, opaqueVertexPos.size() * sizeof(opaqueVertexPos.at(0)), opaqueVertexPos.data(), GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPos), vertexPos.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		// color attribute
		GLuint colorVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, opaqueVertexColor.size() * sizeof(opaqueVertexColor.at(0)), opaqueVertexColor.data(), GL_STATIC_DRAW);


		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);

		Chunk::numOpaqueVertices = opaqueVertexPos.size() / 3;
		opaqueVertexPos.clear();
		opaqueVertexColor.clear();


		// transparent
		// 
		glBindVertexArray(transparentVAO);
		// position attribute
		GLuint transparentPositionVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, transparentPositionVBO);
		glBufferData(GL_ARRAY_BUFFER, transparentVertexPos.size() * sizeof(transparentVertexPos.at(0)), transparentVertexPos.data(), GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPos), vertexPos.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		// color attribute
		GLuint transparentColorVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, transparentColorVBO);
		glBufferData(GL_ARRAY_BUFFER, transparentVertexColor.size() * sizeof(transparentVertexColor.at(0)), transparentVertexColor.data(), GL_STATIC_DRAW);


		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);

		Chunk::numTransparentVertices = transparentVertexPos.size() / 3;
		transparentVertexPos.clear();
		transparentVertexColor.clear();

		glBindVertexArray(0);
		glDeleteBuffers(1, &positionVBO);
		glDeleteBuffers(1, &colorVBO);
		glDeleteBuffers(1, &transparentPositionVBO);
		glDeleteBuffers(1, &transparentColorVBO);
	}

	void Chunk::renderOpaque(std::shared_ptr<Shader> shader) {

		shader->use();

		//glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(WIDTH * x, 0.f, WIDTH * z));
		glBindVertexArray(opaqueVAO);
		//glUniformMatrix4fv(shader->modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, numOpaqueVertices);
		glBindVertexArray(0);
	}
	void Chunk::renderTransparent(std::shared_ptr<Shader> shader) {
		shader->use();

		//glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(WIDTH * x, 0.f, WIDTH * z));

		glBindVertexArray(transparentVAO);
		//glUniformMatrix4fv(shader->modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, numTransparentVertices);
		glBindVertexArray(0);
	}

	void Chunk::setBlock(float x, float y, float z, BlockType block) {
		Chunk::blocks[(int)x][(int)y][(int)z] = block;
	}

	void Chunk::addBlock(float x,float y,float z, BlockType block, std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>>* chunk_map) {
		int globalX = x + Chunk::x * Chunk::WIDTH; int globalZ = z + Chunk::z * Chunk::WIDTH;
		if (y == HEIGHT - 1) {
			Chunk::addTop(globalX, y, globalZ, block);
		}
		else {
			BlockType aboveBlock = blocks[x][y + 1][z];
			if (aboveBlock != block && BlockData::getBlockTransparent(aboveBlock)) {
				Chunk::addTop(globalX, y, globalZ, block);
			}
		}

		if (y == 0) {
			Chunk::addBottom(globalX, y, globalZ, block);
		}
		else {
			BlockType belowBlock = blocks[x][y - 1][z];
			if (belowBlock != block && BlockData::getBlockTransparent(belowBlock)) {
				Chunk::addBottom(globalX, y, globalZ, block);
			}
		}

		BlockType rightBlock = BlockType::Air;
		if (x == WIDTH - 1) {
			auto adjChunk = chunk_map->find(glm::ivec2(this->x + 1, this->z));
			if (adjChunk != chunk_map->end()) rightBlock = adjChunk->second->getBlock(0, y, z);
			//if (adjChunk!=chunk_map->end() && adjChunk->second->getBlock(0, y, z) == BlockType::Air) Chunk::addRight(x, y, z, block);
		}
		else {
			rightBlock = blocks[x + 1][y][z];
		}
		if (rightBlock!=block && BlockData::getBlockTransparent(rightBlock)) {
			Chunk::addRight(globalX, y, globalZ, block);
		}

		BlockType leftBlock = BlockType::Air;
		if (x == 0) {
			auto adjChunk = chunk_map->find(glm::ivec2(this->x - 1, this->z));
			if (adjChunk != chunk_map->end()) leftBlock = adjChunk->second->getBlock(WIDTH-1, y, z);
		}
		else {
			leftBlock = blocks[x - 1][y][z];
		}

		if (leftBlock != block && BlockData::getBlockTransparent(leftBlock)) {
			Chunk::addLeft(globalX, y, globalZ, block);
		}

		BlockType backBlock = BlockType::Air;
		if (z == WIDTH - 1) {
			auto adjChunk = chunk_map->find(glm::ivec2(this->x, this->z+1));
			if (adjChunk != chunk_map->end()) backBlock = adjChunk->second->getBlock(x, y, 0);
		}
		else {
			backBlock = blocks[x][y][z+1];
		}
		if (backBlock != block && BlockData::getBlockTransparent(backBlock)) {
			Chunk::addBack(globalX, y, globalZ, block);
		}

		BlockType frontBlock = BlockType::Air;
		if (z == 0) {
			auto adjChunk = chunk_map->find(glm::ivec2(this->x, this->z - 1));
			if (adjChunk != chunk_map->end()) frontBlock = adjChunk->second->getBlock(x, y, WIDTH - 1);
		}
		else {
			frontBlock = blocks[x][y][z - 1];
		}
		if (frontBlock != block && BlockData::getBlockTransparent(frontBlock)) {
			Chunk::addFront(globalX, y, globalZ, block);
		}

	}

	glm::vec4 randomizeColor(glm::vec4 color, glm::vec3 pos) {

		xxHash hash(0);
		hash = hash.eat(pos.y);
		float var = (uint32_t)hash / static_cast<float>(std::numeric_limits<uint32_t>::max());
		var = var * 0.4f + 0.8f;
		return glm::vec4(color.x * var, color.y * var, color.z * var,color.w);
	}

	void Chunk::addTop(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::array<GLfloat, 18> newVertices= {
			x, y + 1, z,
			x, y + 1, z + 1,
			x + 1,y + 1,z + 1,
			x + 1,y + 1,z + 1,
			x + 1,y + 1,z,
			x,y + 1,z
		} ;
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::array<GLfloat, 24> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};
		if (color.w < 1.0f) {
			transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
			transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
		}
	}
	void Chunk::addLeft(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::array<GLfloat, 18> newVertices = {
			x, y + 1, z,
			x, y + 1, z + 1,
			x,y,z + 1,
			x,y,z + 1,
			x,y,z,
			x,y + 1,z
		};
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::array<GLfloat, 24> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};
		if (color.w < 1.0f) {
			transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
			transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
		}
	}
	void Chunk::addRight(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::array<GLfloat, 18> newVertices = {
			x+1, y + 1, z,
			x + 1, y + 1, z + 1,
			x + 1,y,z + 1,
			x + 1,y,z + 1,
			x + 1,y,z,
			x + 1,y + 1,z
		};
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::array<GLfloat, 24> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};
		if (color.w < 1.0f) {
			transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
			transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
		}
	}
	void Chunk::addFront(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::array<GLfloat, 18> newVertices = {
			x, y + 1, z,
			x + 1, y + 1, z,
			x + 1,y,z,
			x + 1,y,z,
			x,y,z,
			x,y + 1,z
		};
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::array<GLfloat, 24> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};
		if (color.w < 1.0f) {
			transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
			transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
		}
	}
	void Chunk::addBack(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::array<GLfloat, 18> newVertices = {
			x, y + 1, z + 1,
			x + 1, y + 1, z + 1,
			x + 1,y,z + 1,
			x + 1,y,z + 1,
			x,y,z + 1,
			x,y + 1,z+1
		};
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::array<GLfloat, 24> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};

		if (color.w < 1.0f) {
			transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
			transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
		}
	}
	void Chunk::addBottom(GLfloat x, GLfloat y, GLfloat z, BlockType block) {
		std::array<GLfloat, 18> newVertices = {
			x, y , z,
			x, y , z + 1,
			x + 1,y ,z + 1,
			x + 1,y ,z + 1,
			x + 1,y,z,
			x,y ,z
		};
		glm::vec4 color = BlockData::getBlockColor(block);
		color = randomizeColor(color, glm::vec3(x, y, z));
		std::array<GLfloat, 24> newColors = {
			color.x,color.y,color.z, color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
			color.x,color.y,color.z,color.w,
		};
		if (color.w < 1.0f) {
			transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
			transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
		}
	}

	BlockType Chunk::getBlock(int x, int y, int z) {
		return blocks[x][y][z];
	}

	int Chunk::getX() {
		return x;
	}
	int Chunk::getZ() {
		return z;
	}
	bool Chunk::getHasMesh() {
		return hasMesh;
	}
}

