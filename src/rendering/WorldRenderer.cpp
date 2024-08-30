#include "WorldRenderer.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include <src/UI/WorldGenerationGUI.h>
#include <Camera.h>
#include <numbers>


namespace VoxelEngine {

	float VoxelEngine::WorldRenderer::SCREEN_WIDTH = 800;
	float VoxelEngine::WorldRenderer::SCREEN_HEIGHT = 600;


	WorldRenderer::WorldRenderer(std::shared_ptr<World> world)
	{
		WorldRenderer::defaultBlockShader = std::make_shared<Shader>("src/shaders/vertexShader.glsl", "src/shaders/fragmentShader.glsl");
		WorldRenderer::shadowMapShader = std::make_shared<Shader>("src/shaders/depthVertexShader.glsl", "src/shaders/depthFragmentShader.glsl");
		WorldRenderer::world = world;

		/*===============================
			SHADOW MAPPING INITIALIZATION
		=================================*/
		unsigned int depthMapFBO;
		glGenFramebuffers(1, &depthMapFBO);
		WorldRenderer::depthMapFBO = depthMapFBO;

		unsigned int depthMap;
		glGenTextures(1, &depthMap);
		WorldRenderer::depthMap = depthMap;
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

		
	}

	WorldRenderer::~WorldRenderer()
	{
		glDeleteTextures(1, &depthMap);
		glDeleteFramebuffers(1, &depthMapFBO);
	}

	glm::mat4 WorldRenderer::generateLightSpaceMatrix() {
		glm::mat4 lightProjection, lightView;
		float near_plane = 1.0f, far_plane = 600.5f;
		float shadowDistance = mainCamera->viewDistance * 16;
		lightProjection = glm::ortho(-shadowDistance, shadowDistance, -shadowDistance, shadowDistance, near_plane, far_plane);
		//glm::vec3 lightPos = glm::floor(mainCamera->cameraPos) + glm::vec3(10.0f, 30.0f, 0.0f);
		//lightPos.y = 30;
		glm::vec3 lightPos = glm::vec3(0.0f);
		lightPos.x += std::cos(world->getCurrentTime() * 2 * world->invTickRate * std::numbers::pi_v<float>) * 30.0f;
		lightPos.y += std::sin(world->getCurrentTime() * 2 * world->invTickRate * std::numbers::pi_v<float>) * 30.0f;
		glm::vec3 lightDirection(0.0f);

		lightView = glm::lookAt(lightPos,
			lightDirection,
			glm::vec3(0.0f, 1.0f, 0.0f));

		return lightProjection * lightView;
	}
	void WorldRenderer::renderFrame()
	{
		glm::mat4 lightSpaceMatrix = generateLightSpaceMatrix();

		// Shadow Map Rendering
		shadowMapShader->use();
		shadowMapShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderScene();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Main Rendering
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		defaultBlockShader->use();
		mainCamera->updateProjectionMatrixUniform(defaultBlockShader->projectionLoc);
		mainCamera->setViewMatrix();
		mainCamera->updateViewMatrixUniform(defaultBlockShader->viewLoc);
		defaultBlockShader->setVec3("cameraPos", mainCamera->cameraPos);
		defaultBlockShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderScene();

		//Debug Rendering

	}

	void WorldRenderer::renderScene()
	{
		generateMeshes();
		std::lock_guard<std::mutex> lock(world->chunkMap_mutex);
		for (const auto& pair : world->chunk_map) {
			if (pair.second->getHasMesh()) {
				pair.second->renderOpaque(defaultBlockShader);
			}

		}
		for (const auto& pair : world->chunk_map) {
			if (pair.second->getHasMesh()) {
				pair.second->renderTransparent(defaultBlockShader);
			}
		}
	}
	void WorldRenderer::generateMeshes()
	{
		world->updateChunkMap();
		//std::lock_guard<std::mutex> lock(chunkMap_mutex);
	}

}