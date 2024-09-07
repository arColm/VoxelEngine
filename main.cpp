

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <src/shaders/shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <src/rendering/chunk.h>
#include <src/rendering/ChunkLoader.h>
#include <src/UI/FPSCounter.h>
#include <Camera.h>
#include <src/World/world.h>
#include <src/World/IslandChunkGenerator.h>
#include<thread>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <src/UI/WorldGenerationGUI.h>
#include <src/World/PerlinChunkGenerator.h>
#include <src/rendering/WorldRenderer.h>
#include <src/rendering/DebugRenderer.h>

#define DEBUG_ENABLED true

using namespace VoxelEngine;

/*===============================
	FUNCTION DECLARATIONS
=================================*/

void worldUpdateLoop(GLFWwindow* window, std::shared_ptr<World> world);
void renderingLoop(GLFWwindow* window, WorldRenderer* renderer);
void chunkLoadingLoop(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void update(float deltaTime);

/*===============================
	GLOBAL VARIABLES
=================================*/
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


float GUI_WIDTH = 600;




int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WorldRenderer::SCREEN_WIDTH + GUI_WIDTH, WorldRenderer::SCREEN_HEIGHT, "VoxelEngine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); //disables vsync
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Camera cam;
	initializeMainCamera(window,&cam);

	glViewport(0, 0, 800, 600);
	glClearColor(0.25f, 0.60f, 0.62f, 1.0f);

	//if user resizes window, resize viewport
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glPolygonMode(GL_FRONT, GL_FILL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/*===============================
		DEBUG
	=================================*/
	if (DEBUG_ENABLED) {
		VoxelEngine::DebugRenderer::initializeDebugRenderer();
	}

	/*===============================
		SCENE
	=================================*/

	mainCamera->setProjectionMatrix(WorldRenderer::SCREEN_WIDTH, WorldRenderer::SCREEN_HEIGHT);

	PerlinChunkGenerator generator;
	//World world(&generator, 0);
	std::shared_ptr<World> world = std::make_shared<World>(&generator, 0);
	WorldRenderer worldRenderer(world);




	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_ALPHA_TEST);


	glfwMakeContextCurrent(NULL);
	std::thread chunkLoadingThread(&chunkLoadingLoop, window);
	std::thread worldUpdateThread(&worldUpdateLoop, window,world);
	//std::thread renderingThread(&renderingLoop, window,&world);

	renderingLoop(window, &worldRenderer);


	/*===============================
		CLEAN UP
	=================================*/
	//renderingThread.join();
	chunkLoadingThread.join();
	worldUpdateThread.join();
	if(DEBUG_ENABLED) DebugRenderer::releaseResources();
	glfwTerminate();
	return 0;
}

void worldUpdateLoop(GLFWwindow* window, std::shared_ptr<World> world) {
	float tickRate = 20;
	float timeBetweenTicks = 1.0f / tickRate;

	float lastFrameTime = glfwGetTime();
	float deltaTime;
	while (!glfwWindowShouldClose(window)) {
		float startTime = glfwGetTime();
		deltaTime = startTime - lastFrameTime;

		world->tick(deltaTime);
		lastFrameTime = glfwGetTime();
	}
}


void renderingLoop(GLFWwindow* window, WorldRenderer* renderer) {
	glfwMakeContextCurrent(window);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();


	//mainCamera->forceUpdateCurrentChunk();
	while (!glfwWindowShouldClose(window)) {


		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		FPSCounter::updateFPS(deltaTime);
		/*===============================
			INPUT
		=================================*/
		processInput(window);

		//update(deltaTime);
		glfwPollEvents();

		/*===============================
			GUI
		=================================*/
		// (Your code calls glfwPollEvents())
		// ...
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow(); // Show demo window! :)
		VoxelEngine::GUI::ShowWorldGenerationGUIWindow(renderer->world);



		/*===============================
			RENDERING
		=================================*/
		renderer->renderFrame();
		if (DEBUG_ENABLED) {
			DebugRenderer::renderDebug();
		}

		// Rendering GUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		// Swap Buffers
		glfwSwapBuffers(window);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
void chunkLoadingLoop(GLFWwindow* window) {
	mainCamera->forceUpdateCurrentChunk();
	while (!glfwWindowShouldClose(window)) {
		mainCamera->updateCurrentChunk();
	}

}

void update(float deltaTime) {

}

/*
	This method resizes window to input width and height
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	WorldRenderer::SCREEN_WIDTH = width;
	WorldRenderer::SCREEN_HEIGHT = height;
}
/*
	This method processes keyboard input
*/
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		mainCamera->firstMouse = true;
	}
	if (glfwGetMouseButton(window, 0) == GLFW_PRESS && glfwGetInputMode(window,GLFW_CURSOR)==GLFW_CURSOR_NORMAL) {
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		if (xPos < WorldRenderer::SCREEN_WIDTH && yPos < WorldRenderer::SCREEN_HEIGHT) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	mainCamera->moveCamera(window, deltaTime);
}