
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
		Chunk::numWaterVertices = 0;
		Chunk::opaqueVAO = 0;
		Chunk::transparentVAO = 0;
		Chunk::waterVAO = 0;
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
		Chunk::state = State::Empty;
	}
	Chunk::~Chunk() {
		glDeleteVertexArrays(1, &opaqueVAO);
		glDeleteVertexArrays(1, &transparentVAO);
		glDeleteVertexArrays(1, &waterVAO);
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
		Chunk::waterVAO = Loader::createVAO();

	}

	void Chunk::generateMesh() {
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

		// normals attribute
		GLuint normalsVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glBufferData(GL_ARRAY_BUFFER, opaqueVertexNormals.size() * sizeof(opaqueVertexNormals.at(0)), opaqueVertexNormals.data(), GL_STATIC_DRAW);


		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);

		Chunk::numOpaqueVertices = opaqueVertexPos.size() / 3;
		opaqueVertexPos.clear();
		opaqueVertexColor.clear();
		opaqueVertexNormals.clear();


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

		// normals attribute
		GLuint transparentNormalsVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, transparentNormalsVBO);
		glBufferData(GL_ARRAY_BUFFER, transparentVertexNormals.size() * sizeof(transparentVertexNormals.at(0)), transparentVertexNormals.data(), GL_STATIC_DRAW);


		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);

		Chunk::numTransparentVertices = transparentVertexPos.size() / 3;
		transparentVertexPos.clear();
		transparentVertexColor.clear();
		transparentVertexNormals.clear();

		// water
		// 
		glBindVertexArray(waterVAO);
		// position attribute
		GLuint waterPositionVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, waterPositionVBO);
		glBufferData(GL_ARRAY_BUFFER, waterVertexPos.size() * sizeof(waterVertexPos.at(0)), waterVertexPos.data(), GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPos), vertexPos.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		// color attribute
		GLuint waterColorVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, waterColorVBO);
		glBufferData(GL_ARRAY_BUFFER, waterVertexColor.size() * sizeof(waterVertexColor.at(0)), waterVertexColor.data(), GL_STATIC_DRAW);


		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);

		// normals attribute
		GLuint waterNormalsVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, waterNormalsVBO);
		glBufferData(GL_ARRAY_BUFFER, waterVertexNormals.size() * sizeof(waterVertexNormals.at(0)), waterVertexNormals.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);

		Chunk::numWaterVertices = waterVertexPos.size() / 3;
		waterVertexPos.clear();
		waterVertexColor.clear();
		waterVertexNormals.clear();

		glBindVertexArray(0);
		glDeleteBuffers(1, &positionVBO);
		glDeleteBuffers(1, &colorVBO);
		glDeleteBuffers(1, &normalsVBO);
		glDeleteBuffers(1, &transparentPositionVBO);
		glDeleteBuffers(1, &transparentColorVBO);
		glDeleteBuffers(1, &transparentNormalsVBO);
		glDeleteBuffers(1, &waterPositionVBO);
		glDeleteBuffers(1, &waterColorVBO);
		glDeleteBuffers(1, &waterNormalsVBO);
	}

	void Chunk::renderOpaque() {
		glBindVertexArray(opaqueVAO);
		glDrawArrays(GL_TRIANGLES, 0, numOpaqueVertices);
		glBindVertexArray(0);
	}
	void Chunk::renderTransparent() {
		glBindVertexArray(transparentVAO);
		glDrawArrays(GL_TRIANGLES, 0, numTransparentVertices);
		glBindVertexArray(0);
	}
	void Chunk::renderWater() {
		glBindVertexArray(waterVAO);
		glDrawArrays(GL_TRIANGLES, 0, numWaterVertices);
		glBindVertexArray(0);
	}
	void Chunk::setBlock(float x, float y, float z, BlockType block, bool override) {
		if(override || blocks[(int)x][(int)y][(int)z] == BlockType::Air) Chunk::blocks[(int)x][(int)y][(int)z] = block;
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
		std::array<GLfloat, 18> normals = {
			0,1,0,
			0,1,0,
			0,1,0,
			0,1,0,
			0,1,0,
			0,1,0,
		};
		if (color.w < 1.0f) {
			if (block == BlockType::Water) {
				waterVertexPos.insert(waterVertexPos.end(), newVertices.begin(), newVertices.end());
				waterVertexColor.insert(waterVertexColor.end(), newColors.begin(), newColors.end());
				waterVertexNormals.insert(waterVertexNormals.end(), normals.begin(), normals.end());
			}
			else {
				transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
				transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
				transparentVertexNormals.insert(transparentVertexNormals.end(), normals.begin(), normals.end());
			}
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
			opaqueVertexNormals.insert(opaqueVertexNormals.end(), normals.begin(), normals.end());
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
		std::array<GLfloat, 18> normals = {
			-1,0,0,
			-1,0,0,
			-1,0,0,
			-1,0,0,
			-1,0,0,
			-1,0,0,
		};
		if (color.w < 1.0f) {
			if (block == BlockType::Water) {
				waterVertexPos.insert(waterVertexPos.end(), newVertices.begin(), newVertices.end());
				waterVertexColor.insert(waterVertexColor.end(), newColors.begin(), newColors.end());
				waterVertexNormals.insert(waterVertexNormals.end(), normals.begin(), normals.end());
			}
			else {
				transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
				transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
				transparentVertexNormals.insert(transparentVertexNormals.end(), normals.begin(), normals.end());
			}
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
			opaqueVertexNormals.insert(opaqueVertexNormals.end(), normals.begin(), normals.end());
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
		std::array<GLfloat, 18> normals = {
			1,0,0,
			1,0,0,
			1,0,0,
			1,0,0,
			1,0,0,
			1,0,0,
		};
		if (color.w < 1.0f) {
			if (block == BlockType::Water) {
				waterVertexPos.insert(waterVertexPos.end(), newVertices.begin(), newVertices.end());
				waterVertexColor.insert(waterVertexColor.end(), newColors.begin(), newColors.end());
				waterVertexNormals.insert(waterVertexNormals.end(), normals.begin(), normals.end());
			}
			else {
				transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
				transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
				transparentVertexNormals.insert(transparentVertexNormals.end(), normals.begin(), normals.end());
			}
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
			opaqueVertexNormals.insert(opaqueVertexNormals.end(), normals.begin(), normals.end());
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
		std::array<GLfloat, 18> normals = {
			0,0,-1,
			0,0,-1,
			0,0,-1,
			0,0,-1,
			0,0,-1,
			0,0,-1,
		};
		if (color.w < 1.0f) {
			if (block == BlockType::Water) {
				waterVertexPos.insert(waterVertexPos.end(), newVertices.begin(), newVertices.end());
				waterVertexColor.insert(waterVertexColor.end(), newColors.begin(), newColors.end());
				waterVertexNormals.insert(waterVertexNormals.end(), normals.begin(), normals.end());
			}
			else {
				transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
				transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
				transparentVertexNormals.insert(transparentVertexNormals.end(), normals.begin(), normals.end());
			}
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
			opaqueVertexNormals.insert(opaqueVertexNormals.end(), normals.begin(), normals.end());
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
		std::array<GLfloat, 18> normals = {
			0,0,1,
			0,0,1,
			0,0,1,
			0,0,1,
			0,0,1,
			0,0,1,
		};
		if (color.w < 1.0f) {
			if (block == BlockType::Water) {
				waterVertexPos.insert(waterVertexPos.end(), newVertices.begin(), newVertices.end());
				waterVertexColor.insert(waterVertexColor.end(), newColors.begin(), newColors.end());
				waterVertexNormals.insert(waterVertexNormals.end(), normals.begin(), normals.end());
			}
			else {
				transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
				transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
				transparentVertexNormals.insert(transparentVertexNormals.end(), normals.begin(), normals.end());
			}
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
			opaqueVertexNormals.insert(opaqueVertexNormals.end(), normals.begin(), normals.end());
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
		std::array<GLfloat, 18> normals = {
			0,-1,0,
			0,-1,0,
			0,-1,0,
			0,-1,0,
			0,-1,0,
			0,-1,0,
		};
		if (color.w < 1.0f) {
			if (block == BlockType::Water) {
				waterVertexPos.insert(waterVertexPos.end(), newVertices.begin(), newVertices.end());
				waterVertexColor.insert(waterVertexColor.end(), newColors.begin(), newColors.end());
				waterVertexNormals.insert(waterVertexNormals.end(), normals.begin(), normals.end());
			}
			else {
				transparentVertexPos.insert(transparentVertexPos.end(), newVertices.begin(), newVertices.end());
				transparentVertexColor.insert(transparentVertexColor.end(), newColors.begin(), newColors.end());
				transparentVertexNormals.insert(transparentVertexNormals.end(), normals.begin(), normals.end());
			}
		}
		else {
			opaqueVertexPos.insert(opaqueVertexPos.end(), newVertices.begin(), newVertices.end());
			opaqueVertexColor.insert(opaqueVertexColor.end(), newColors.begin(), newColors.end());
			opaqueVertexNormals.insert(opaqueVertexNormals.end(), normals.begin(), normals.end());
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
	void Chunk::updateState(State newState)
	{
		Chunk::state = newState;
	}
	Chunk::State Chunk::getState() {
		return state;
	}

	int Chunk::getGroundHeight(int x, int z) {
		if (x >= Chunk::WIDTH || x < 0 || z >= Chunk::WIDTH || z < 0) return 0;
		//TODO : REFACTOR THIS
		
		for (int y = HEIGHT - 1; y >= 0; y--) {
			BlockType block = blocks[x][y][z];
			if (block == BlockType::Grass ||
				block == BlockType::Dirt ||
				block == BlockType::Snow) return y + 1;
		}
		return 0;


	}
}

