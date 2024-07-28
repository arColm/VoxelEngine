
#include <glad/glad.h>
#include "Camera.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace VoxelEngine {
	//boost::signals2::signal<void()> enterNewChunkEvent;
	Camera* mainCamera = nullptr;
	glm::mat4 viewMatrix = glm::mat4(0);
	glm::mat4 projectionMatrix = glm::mat4(0);

	Camera::Camera() { }
	Camera::~Camera() { }

	void Camera::updateCurrentChunk() {
		
		glm::ivec2 nextChunk = glm::ivec2((int)abs(cameraPos.x) / Chunk::getWidth(), (int)abs(cameraPos.z) / Chunk::getWidth());
		if (cameraPos.x < 0) nextChunk.x = -(nextChunk.x + 1);
		if (cameraPos.z < 0) nextChunk.y = -(nextChunk.y + 1);
		if (nextChunk.x != currentChunk.x || nextChunk.y != currentChunk.y) {
			currentChunk = nextChunk;
			//enterNewChunkEvent.invoke(nextChunk);
			//enterNewChunkEvent(currentChunk, viewDistance);
			enterNewChunkEvent(nextChunk,viewDistance);
			//std::cout << cameraPos.x << '-' << cameraPos.z << ' ' << currentChunk.x << '-' << currentChunk.y << std::endl;
		}
	}

	void setViewMatrix(unsigned int viewLoc) {
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(mainCamera->cameraPos, mainCamera->cameraPos + mainCamera->cameraFront, mainCamera->cameraUp);
		viewMatrix = view;
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}
	void setProjectionMatrix(float SCREEN_WIDTH, float SCREEN_HEIGHT) {
		glm::mat4 projection = glm::perspective(glm::radians(mainCamera->fov), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
		projectionMatrix = projection;
	}

	void updateProjectionMatrix(unsigned int projectionLoc) {
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	}



	void Camera::moveCamera(GLFWwindow* window, float deltaTime) {
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


	void initializeCamera(GLFWwindow* window,Camera* camera) {
		mainCamera = camera;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
	}

	/*
		This method processes mouse movement
	*/
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
		if (mainCamera->firstMouse) // initially set to true
		{
			mainCamera->lastX = (float) xpos;
			mainCamera->lastY = (float) ypos;
			mainCamera->firstMouse = false;
		}
		//calculate mouse offset
		float xOffset = (float)xpos - mainCamera->lastX;
		float yOffset = mainCamera->lastY - (float)ypos;
		mainCamera->lastX = (float)xpos;
		mainCamera->lastY = (float)ypos;

		xOffset *= mainCamera->sensitivity;
		yOffset *= mainCamera->sensitivity;

		mainCamera->yaw += xOffset;
		mainCamera->pitch += yOffset;

		if (mainCamera->pitch > 89.0f) mainCamera->pitch = 89.0f;
		if (mainCamera->pitch < -89.0f) mainCamera->pitch = -89.0f;

		glm::vec3 direction{};
		direction.x = cos(glm::radians(mainCamera->yaw)) * cos(glm::radians(mainCamera->pitch));
		direction.y = sin(glm::radians(mainCamera->pitch));
		direction.z = sin(glm::radians(mainCamera->yaw)) * cos(glm::radians(mainCamera->pitch));

		mainCamera->cameraFront = glm::normalize(direction);
	}
	/*
		This method processes scrollwheel movement
	*/
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		mainCamera->fov -= (float)yoffset;
		if (mainCamera->fov < 1.0f)
			mainCamera->fov = 1.0f;
		if (mainCamera->fov > 45.0f)
			mainCamera->fov = 45.0f;
	}
}
