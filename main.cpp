

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <src/shaders/shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <src/rendering/chunk.h>
#include <src/rendering/ChunkLoader.h>

using namespace VoxelEngine;

/*===============================
	FUNCTION DECLARATIONS
=================================*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void update(float deltaTime);

/*===============================
	GLOBAL VARIABLES
=================================*/
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//mouse
float lastX = 400, lastY = 300; //last position of the mouse
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;
const float sensitivity = 0.1f;
/*===============================
	DEFAULT CAMERA
=================================*/
glm::vec3 cameraPos = glm::vec3(0.0f, 15.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0);
const float cameraSpeed = 10.0f;

float fov = 45.0f;


int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "VoxelEngine", NULL, NULL);
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glViewport(0, 0, 800, 600);

	//if user resizes window, resize viewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/*===============================
		SCENE
	=================================*/
	Chunk chunk(-1, -1);
	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
			chunk.setBlock(x, 12, z, BlockType::Dirt);
		}
	}	
	Chunk chunk2(0, 0);
	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
			chunk2.setBlock(x, 15, z, BlockType::Dirt);
		}
	}
	Shader defaultShader("src/shaders/vertexShader.glsl", "src/shaders/fragmentShader.glsl");
	ChunkLoader chunkLoader(&defaultShader);
	chunkLoader.addChunk(-1,-1,&chunk);
	chunkLoader.addChunk(0,0,&chunk2);
	defaultShader.use();



	glm::mat4 projection = glm::mat4(1.0f);
	unsigned int projectionLoc = glGetUniformLocation(defaultShader.ID, "projection");
	unsigned int viewLoc = glGetUniformLocation(defaultShader.ID, "view");
	projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

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
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

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
}
/*
	This method processes keyboard input
*/
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraFront * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraFront * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraUp * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraUp * deltaTime;
	}
}
/*
	This method processes mouse movement
*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	//calculate mouse offset
	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraFront = glm::normalize(direction);
}
/*
	This method processes scrollwheel movement
*/
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}