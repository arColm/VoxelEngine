#include "WorldRenderer.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include <src/UI/WorldGenerationGUI.h>
#include <Camera.h>
#include <numbers>
#include "DebugRenderer.h"


namespace VoxelEngine {

	float VoxelEngine::WorldRenderer::SCREEN_WIDTH = 800;
	float VoxelEngine::WorldRenderer::SCREEN_HEIGHT = 600;



	WorldRenderer::WorldRenderer(std::shared_ptr<World> world)
	{
		WorldRenderer::defaultBlockShader = std::make_shared<Shader>("src/shaders/vertexShader.glsl", "src/shaders/fragmentShader.glsl");
		WorldRenderer::shadowMapShader = std::make_shared<Shader>("src/shaders/depthVertexShader.glsl", "src/shaders/depthFragmentShader.glsl");
		WorldRenderer::world = world;


		WorldRenderer::debugRendererConnection = VoxelEngine::DebugRenderer::drawDebugEvent.connect(
			boost::bind(&WorldRenderer::onDrawDebug, this));
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
		WorldRenderer::debugRendererConnection.disconnect();
	}

	glm::mat4 WorldRenderer::generateLightSpaceMatrix() {
		float shadowDistance = mainCamera->viewDistance * 16;
		glm::mat4 lightProjection, lightView;
		sunPos = mainCamera->cameraPos;
		sunPos.x += std::cos(world->getCurrentTime() * 2 * world->invTickRate * std::numbers::pi_v<float>) * shadowDistance;
		sunPos.y = std::sin(world->getCurrentTime() * 2 * world->invTickRate * std::numbers::pi_v<float>) * sunMaxHeight;

		float near_plane = 1.0f, far_plane = 2000.5f;
		lightProjection = glm::ortho(-shadowDistance, shadowDistance, -shadowDistance, shadowDistance, near_plane, far_plane);
		//glm::vec3 lightPos = glm::floor(mainCamera->cameraPos) + glm::vec3(10.0f, 30.0f, 0.0f);
		//lightPos.y = 30;

		glm::vec3 sunTarget;
		sunTarget = mainCamera->cameraPos;
		sunTarget.y = Chunk::HEIGHT * 0.5f;
		sunLightDirection = glm::normalize(sunTarget - sunPos);
		//lightDirection.y = -shadowDistance*2;
		lightView = glm::lookAt(sunPos,
			sunTarget,
			glm::vec3(-sunLightDirection.y, -sunLightDirection.x, 0.0f));

		//sunLightDirection = glm::normalize(sunTarget - sunPos);

		return lightProjection * lightView;
	}
	void WorldRenderer::renderFrame()
	{
		// Shadow Map Rendering
		glm::mat4 lightSpaceMatrix = generateLightSpaceMatrix();

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
		defaultBlockShader->setFloat("sunHeight", sunPos.y/sunMaxHeight);
		defaultBlockShader->setVec3("sunLightDirection", sunLightDirection);
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

	void WorldRenderer::onDrawDebug()
	{
		DebugRenderer::drawCube(sunPos, glm::vec3(1, 1, 0));
	}
}