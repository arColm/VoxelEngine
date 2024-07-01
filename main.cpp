

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <src/shaders/shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <src/rendering/chunk.h>
#include <src/rendering/ChunkLoader.h>
#include <Camera.h>

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

	Camera::initializeCamera(window);

	glViewport(0, 0, 800, 600);

	//if user resizes window, resize viewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glPolygonMode(GL_FRONT, GL_FILL);

	/*===============================
		SCENE
	=================================*/
	Chunk chunk(-1, -1);
	for (int x = 0; x < 16; x+=2) {
		for (int z = 0; z < 16; z+=2) {
			chunk.setBlock(x, x+z, z, BlockType::Dirt);
		}
	}	
	Chunk chunk2(0, 0);
	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
			chunk2.setBlock(x, 12, z, BlockType::Dirt);
		}
	}
	Shader defaultShader("src/shaders/vertexShader.glsl", "src/shaders/fragmentShader.glsl");
	ChunkLoader chunkLoader(&defaultShader);
	chunkLoader.addChunk(-1,-1,&chunk);
	chunkLoader.addChunk(0,0,&chunk2);
	defaultShader.use();



	Camera::setProjectionMatrix(defaultShader.projectionLoc, SCREEN_WIDTH, SCREEN_HEIGHT);

	glEnable(GL_DEPTH_TEST);

	chunkLoader.loadChunks();
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		std::cout << (1.0f / deltaTime) << '\n';
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


		chunkLoader.renderChunks();


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
	Camera::moveCamera(window, deltaTime);
}