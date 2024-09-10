#include "WorldRenderer.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include <src/UI/WorldGenerationGUI.h>
#include <Camera.h>
#include <numbers>
#include "DebugRenderer.h"
#include "loader.h"


namespace VoxelEngine {

	float VoxelEngine::WorldRenderer::SCREEN_WIDTH = 800;
	float VoxelEngine::WorldRenderer::SCREEN_HEIGHT = 600;


	const std::array<GLfloat, 108> skyboxVertices = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f

	};

	std::array<GLfloat, 36> sunMoonVertices;
	const std::array<GLfloat, 36> sunMoonColors = {
		0.9608f,0.7725f, 0.3373f,
		0.9608f,0.7725f, 0.3373f,
		0.9608f,0.7725f, 0.3373f,
		0.9608f,0.7725f, 0.3373f,
		0.9608f,0.7725f, 0.3373f,
		0.9608f,0.7725f, 0.3373f,

		0.6706f, 0.6706f,0.6706f,
		0.6706f, 0.6706f,0.6706f,
		0.6706f, 0.6706f,0.6706f,
		0.6706f, 0.6706f,0.6706f,
		0.6706f, 0.6706f,0.6706f,
		0.6706f, 0.6706f,0.6706f,
	};



	WorldRenderer::WorldRenderer(std::shared_ptr<World> world)
	{
		WorldRenderer::defaultBlockShader = std::make_shared<Shader>("src/shaders/vertexShader.glsl", "src/shaders/fragmentShader.glsl");
		WorldRenderer::shadowMapShader = std::make_shared<Shader>("src/shaders/depthVertexShader.glsl", "src/shaders/depthFragmentShader.glsl");
		WorldRenderer::skyboxShader = std::make_unique<Shader>("src/shaders/skyboxVertexShader.glsl", "src/shaders/skyboxFragmentShader.glsl");
		WorldRenderer::sunMoonShader = std::make_unique<Shader>("src/shaders/sunMoonVertexShader.glsl", "src/shaders/sunMoonFragmentShader.glsl");
		WorldRenderer::cloudShader = std::make_unique<Shader>("src/shaders/cloudVertexShader.glsl", "src/shaders/cloudFragmentShader.glsl");
		WorldRenderer::waterShader = std::make_unique<Shader>("src/shaders/waterVertexShader.glsl", "src/shaders/waterFragmentShader.glsl");
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

		/*===============================
			SKYBOX INITIALIZATION
		=================================*/

		WorldRenderer::skyboxVAO = VoxelEngine::Loader::createVAO();
		glBindVertexArray(skyboxVAO);

		unsigned int skyboxVBO = VoxelEngine::Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

		glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(skyboxVertices.at(0)), skyboxVertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

		glBindVertexArray(0);
		glDeleteBuffers(1,&skyboxVBO);


		WorldRenderer::sunMoonVAO = VoxelEngine::Loader::createVAO();
		glBindVertexArray(sunMoonVAO);
		float sunDistance = Chunk::WIDTH * mainCamera->viewDistance * 1.5f;
		float sunMaxX = mainCamera->viewDistance;
		float moonMaxX = sunMaxX *0.5f;
		sunMoonVertices = {
			sunDistance, -sunMaxX,sunMaxX,
			sunDistance, sunMaxX,sunMaxX,
			sunDistance, -sunMaxX,-sunMaxX,
			sunDistance, -sunMaxX, -sunMaxX,
			sunDistance, sunMaxX, -sunMaxX,
			sunDistance, sunMaxX, sunMaxX,

			-sunDistance, moonMaxX,moonMaxX,
			-sunDistance, -moonMaxX,moonMaxX,
			-sunDistance, -moonMaxX,-moonMaxX,
			-sunDistance, -moonMaxX, -moonMaxX,
			-sunDistance, moonMaxX, -moonMaxX,
			-sunDistance, moonMaxX, moonMaxX,
		};
		unsigned int sunMoonPosVBO = VoxelEngine::Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, sunMoonPosVBO);

		glBufferData(GL_ARRAY_BUFFER, sunMoonVertices.size() * sizeof(sunMoonVertices.at(0)), sunMoonVertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		
		unsigned int sunMoonColorVBO = VoxelEngine::Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, sunMoonColorVBO);
		glBufferData(GL_ARRAY_BUFFER, sunMoonColors.size() * sizeof(sunMoonColors.at(0)), sunMoonColors.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glDeleteBuffers(1, &sunMoonPosVBO);
		glDeleteBuffers(1, &sunMoonColorVBO);


		WorldRenderer::cloudVAO = VoxelEngine::Loader::createVAO();
		glBindVertexArray(cloudVAO);

		WorldRenderer::cloudDistance = mainCamera->viewDistance * Chunk::WIDTH*1.5f;
		WorldRenderer::cloudHeight = cloudDistance * 0.15f;

		const std::array<GLfloat, 30> cloudPos = {
			-cloudDistance,cloudHeight,cloudDistance, 0.f, 1.f,
			cloudDistance,cloudHeight,cloudDistance, 1.f, 1.f,
			-cloudDistance,cloudHeight,-cloudDistance,0.f, 0.f,
			-cloudDistance,cloudHeight,-cloudDistance, 0.f, 0.f,
			cloudDistance,cloudHeight,cloudDistance, 1.f, 1.f,
			cloudDistance,cloudHeight,-cloudDistance, 1.f, 0.f,
		};

		unsigned int cloudVBO = VoxelEngine::Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, cloudVBO);

		glBufferData(GL_ARRAY_BUFFER, cloudPos.size() * sizeof(cloudPos.at(0)), cloudPos.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glDeleteBuffers(1, &cloudVBO);
		
	}

	WorldRenderer::~WorldRenderer()
	{
		glDeleteTextures(1, &depthMap);
		glDeleteFramebuffers(1, &depthMapFBO);

		glDeleteVertexArrays(1, &skyboxVAO);
		glDeleteVertexArrays(1, &sunMoonVAO);
		glDeleteVertexArrays(1, &cloudVAO);

		WorldRenderer::debugRendererConnection.disconnect();
	}

	glm::mat4 WorldRenderer::generateLightSpaceMatrix() {
		float shadowDistance = mainCamera->viewDistance * 16;
		glm::mat4 lightProjection, lightView;
		sunPos = mainCamera->cameraPos;
		sunPos.x += std::cos(world->getCurrentTime() * 2 * world->invTimePerDay * std::numbers::pi_v<float>) * shadowDistance;
		sunPos.y = std::sin(world->getCurrentTime() * 2 * world->invTimePerDay * std::numbers::pi_v<float>) * sunMaxHeight;

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
		sunHeightNormalized = sunPos.y / sunMaxHeight;
		if(sunHeightNormalized<=0) {
			fogColor = glm::mix(glm::vec3(0.f), glm::vec3(0.878f, 0.482f, 0.027f), std::pow(std::clamp(sunHeightNormalized, -1.f, 0.f)+1,10));
		}
		else {
			fogColor = glm::mix(glm::vec3(0.878f, 0.482f, 0.027f), glm::vec3(1.f), std::sqrt(std::clamp(sunHeightNormalized, 0.f, 1.0f)));
		}


		// Shadow Map Rendering
		glm::mat4 lightSpaceMatrix = generateLightSpaceMatrix();

		shadowMapShader->use();
		shadowMapShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderSceneBlocks();
		renderWater();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// Main Rendering
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		// Render Scene
		mainCamera->setViewMatrix();

		defaultBlockShader->use();
		mainCamera->updateProjectionMatrixUniform(defaultBlockShader->projectionLoc);
		mainCamera->updateViewMatrixUniform(defaultBlockShader->viewLoc);
		defaultBlockShader->setVec3("cameraPos", mainCamera->cameraPos);
		defaultBlockShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		defaultBlockShader->setFloat("sunHeight", sunHeightNormalized);
		defaultBlockShader->setVec3("sunLightDirection", sunLightDirection);
		defaultBlockShader->setVec3("fogColor", fogColor);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderSceneBlocks();

		waterShader->use();
		mainCamera->updateProjectionMatrixUniform(waterShader->projectionLoc);
		mainCamera->updateViewMatrixUniform(waterShader->viewLoc);
		waterShader->setVec3("cameraPos", mainCamera->cameraPos);
		waterShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		waterShader->setFloat("sunHeight", sunHeightNormalized);
		waterShader->setFloat("time", world->getTotalTime());
		waterShader->setVec3("sunLightDirection", sunLightDirection);
		waterShader->setVec3("fogColor", fogColor);

		renderWater();
		// Skybox Rendering

		renderSkybox();
		//Debug Rendering
	}


	void WorldRenderer::renderSceneBlocks()
	{
		generateMeshes();
		std::lock_guard<std::mutex> lock(world->chunkMap_mutex);
		for (const auto& pair : world->chunk_map) {
			if (pair.second->getHasMesh()) {
				pair.second->renderOpaque();
			}

		}
		for (const auto& pair : world->chunk_map) {
			if (pair.second->getHasMesh()) {
				pair.second->renderTransparent();
			}
		}
	}

	void WorldRenderer::renderWater() {
		generateMeshes();
		for (const auto& pair : world->chunk_map) {
			if (pair.second->getHasMesh()) {
				pair.second->renderWater();
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
		//DebugRenderer::drawCube(sunPos, glm::vec3(1, 1, 0));
	}

	void WorldRenderer::renderSkybox() 
	{

		sunMoonShader->use();
		mainCamera->updateProjectionMatrixUniform(sunMoonShader->projectionLoc);
		mainCamera->updateViewMatrixUniform(sunMoonShader->viewLoc);
		sunMoonShader->setVec3("cameraPos", mainCamera->cameraPos);
		glm::mat4 model(1.0f);
		model = glm::translate(model, mainCamera->cameraPos);
		model = glm::rotate(model, -(world->getCurrentTime() / world->TIME_PER_DAY) * 2 * std::numbers::pi_v<float>, glm::vec3(0, 0, -1));
		sunMoonShader->setMat4("model", model);
		glBindVertexArray(sunMoonVAO);
		glDrawArrays(GL_TRIANGLES, 0, sunMoonVertices.size() / 3);
		glBindVertexArray(0);


		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader->use();
		mainCamera->updateProjectionMatrixUniform(skyboxShader->projectionLoc);
		mainCamera->updateViewMatrixUniform(skyboxShader->viewLoc);
		skyboxShader->setVec3("cameraPos", mainCamera->cameraPos);
		skyboxShader->setFloat("sunHeight", sunHeightNormalized);
		skyboxShader->setVec3("fogColor", fogColor);
		glBindVertexArray(skyboxVAO);
		glDrawArrays(GL_TRIANGLES, 0, skyboxVertices.size()/3);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default


		cloudShader->use();
		mainCamera->updateProjectionMatrixUniform(cloudShader->projectionLoc);
		mainCamera->updateViewMatrixUniform(cloudShader->viewLoc);
		cloudShader->setVec3("cameraPos", mainCamera->cameraPos);
		model = glm::mat4(1.0f);
		model = glm::translate(model, mainCamera->cameraPos);
		cloudShader->setMat4("model", model);
		cloudShader->setFloat("time", world->getTotalTime());
		cloudShader->setFloat("sunHeight", sunHeightNormalized);

		glBindVertexArray(cloudVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


	}
}