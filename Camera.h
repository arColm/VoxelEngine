#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include "src/rendering/Chunk.h"
#include "boost/signals2.hpp"

namespace VoxelEngine {


	class Camera {
	private:
		bool currentChunkInvalid = false;
	public:
		const float baseCameraSpeed = 20.0f;

		glm::vec3 cameraPos = glm::vec3(16.0f, 14.0f, 3.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0);
		float cameraSpeed = baseCameraSpeed;

		float fov = 45.0f;

		//this is inaccurate, but gets changed instantly
		glm::vec2 currentChunk = glm::vec2((int)cameraPos.x / Chunk::WIDTH, (int)cameraPos.z / Chunk::WIDTH);
		int viewDistance = 9;


		//mouse
		float lastX = 400, lastY = 300; //last position of the mouse
		float yaw = -90.0f;
		float pitch = 0.0f;
		bool firstMouse = true;
		const float sensitivity = 0.1f;


		glm::mat4 viewMatrix = glm::mat4(0);
		glm::mat4 projectionMatrix = glm::mat4(0);

		boost::signals2::signal<void(glm::vec2, int) > enterNewChunkEvent;


		Camera();
		~Camera();
		void updateCurrentChunk();
		void forceUpdateCurrentChunk();
		void moveCamera(GLFWwindow* window, float deltaTime);

		void setViewMatrix();
		void updateViewMatrixUniform(unsigned int viewLoc);
		void setProjectionMatrix(float SCREEN_WIDTH, float SCREEN_HEIGHT);
		void updateProjectionMatrixUniform(unsigned int projectionLoc);
	};

	extern Camera* mainCamera;



	void initializeMainCamera(GLFWwindow* window,Camera* cam);
	void updateViewMatrixUniform(unsigned int viewLoc);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


}


#endif