

#include "ChunkLoader.h"
#include <Camera.h>
#include <thread>
#include "Renderer.h"

namespace VoxelEngine{
	ChunkLoader::ChunkLoader() : ChunkLoader(std::make_shared<Shader>("src/shaders/vertexShader.glsl", "src/shaders/fragmentShader.glsl")) {

	}
	ChunkLoader::ChunkLoader(std::shared_ptr<Shader> shader) {
		ChunkLoader::shader = shader;


		// setup shadow mapping texture
		unsigned int depthMapFBO;
		glGenFramebuffers(1, &depthMapFBO);
		ChunkLoader::depthMapFBO = depthMapFBO;

		unsigned int depthMap;
		glGenTextures(1, &depthMap);
		ChunkLoader::depthMap = depthMap;
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); // TODO: change 1024 to generics (SHADOW_WIDTH/SHADOW_HEIGHT)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		updateProjectionMatrix(shader->projectionLoc);
		ChunkLoader::lightShader = std::make_shared<Shader>("src/shaders/depthVertexShader.glsl", "src/shaders/depthFragmentShader.glsl");

		lightShader->use();

		float near_plane = 1.0f, far_plane = 700.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		glm::mat4 lightView = glm::lookAt(glm::vec3(60.0f, 60.0f, 0.0f),
			glm::vec3(60.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		lightShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shader->use();
		shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shader->setInt("shadowMap", 0);
	}
	ChunkLoader::~ChunkLoader() { }

	void ChunkLoader::loadChunks() {
		std::vector < std::thread> threads;

		for(const auto& pair : chunk_map) {
			threads.push_back(std::thread(&Chunk::load, pair.second, &chunk_map));
			//pair.second->load(&chunk_map);
		}
		for (auto& t : threads) {
			t.join();
		}
		/*for (const auto& pair : chunk_map) {
			pair.second->loadBlocks();
		}*/
	}
	void ChunkLoader::renderMeshes() {
		for (const auto& pair : chunk_map) {
			if (pair.second->getHasMesh()) {
				pair.second->renderOpaque(shader);
			}

		}
		for (const auto& pair : chunk_map) {
			if (pair.second->getHasMesh()) {
				pair.second->renderTransparent(shader);
			}
		}
	}
	void ChunkLoader::renderChunks() {
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 200.5f;
		lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
		lightView = glm::lookAt(glm::vec3(10.0f, 30.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f));

		//lightProjection = VoxelEngine::projectionMatrix;
		//lightView = VoxelEngine::viewMatrix;
		lightSpaceMatrix = lightProjection * lightView;

		// Light Rendering


		lightShader->use();
		lightShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, Renderer::SHADOW_WIDTH, Renderer::SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderMeshes();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Main Rendering
		glViewport(0, 0, VoxelEngine::Renderer::SCREEN_WIDTH, VoxelEngine::Renderer::SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->use();
		updateProjectionMatrix(shader->projectionLoc);
		setViewMatrix(shader->viewLoc);
		shader->setVec3("cameraPos", mainCamera->cameraPos);
		shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderMeshes();
	}

	void ChunkLoader::addChunk(int x, int y, std::shared_ptr<Chunk> chunk) {
		ChunkLoader::chunk_map.insert_or_assign(glm::ivec2(x, y), chunk);
	}
	void ChunkLoader::removeChunk(int x, int y) {

	}
}