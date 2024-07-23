

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
#include <src/World/FlatChunkGenerator.h>
#include <src/World/PyramidChunkGenerator.h>
#include <src/World/PerlinChunkGenerator.h>
#include <src/World/IslandChunkGenerator.h>

using namespace VoxelEngine;

/*===============================
	FUNCTION DECLARATIONS
=================================*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void update(float deltaTime);

/*===============================
	GLOBAL VARIABLES
=================================*/
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float SCREEN_WIDTH = 800;
float SCREEN_HEIGHT = 600;


int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "VoxelEngine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//glfwSwapInterval(0); //disables vsync
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Camera cam;
	initializeCamera(window,&cam);

	glViewport(0, 0, 800, 600);

	//if user resizes window, resize viewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glPolygonMode(GL_FRONT, GL_FILL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/*===============================
		SCENE
	=================================*/
	//Shader defaultShader("src/shaders/vertexShader.glsl", "src/shaders/fragmentShader.glsl");
	//ChunkLoader chunkLoader(&defaultShader);
	//chunkLoader.addChunk(-1,-1,&chunk);
	//chunkLoader.addChunk(0,0,&chunk2);

	setProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT);

	IslandChunkGenerator generator;
	World world(&generator, 0);




	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	//chunkLoader.loadChunks();
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		FPSCounter::updateFPS(deltaTime);
		/*===============================
			INPUT
		=================================*/
		processInput(window);
		/*===============================
			RENDERING
		=================================*/
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update(deltaTime);
		//set view model


		mainCamera->updateCurrentChunk();
		world.renderChunks();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	/*===============================
		CLEAN UP
	=================================*/
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

void update(float deltaTime) {

}

/*
	This method resizes window to input width and height
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}
/*
	This method processes keyboard input
*/
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	mainCamera->moveCamera(window, deltaTime);
}