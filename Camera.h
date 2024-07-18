#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <src/Utils/Event.h>
#include "src/rendering/Chunk.h"
#include "boost/signals2.hpp"

namespace VoxelEngine {
	//(x,z,viewDistance)
	//static Event<int,int> enterNewChunkEvent;


	class Camera {

	public:

		glm::vec3 cameraPos = glm::vec3(0.0f, 7.0f, 3.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0);
		const float cameraSpeed = 10.0f;

		float fov = 45.0f;

		//this is inaccurate, but gets changed instantly
		glm::vec2 currentChunk = glm::vec2((int)cameraPos.x / Chunk::getWidth(), (int)cameraPos.z / Chunk::getWidth());
		int viewDistance = 6;


		//mouse
		float lastX = 400, lastY = 300; //last position of the mouse
		float yaw = -90.0f;
		float pitch = 0.0f;
		bool firstMouse = true;
		const float sensitivity = 0.1f;



		boost::signals2::signal<void(glm::vec2, int) > enterNewChunkEvent;


		Camera();
		~Camera();
		void updateCurrentChunk();
		void moveCamera(GLFWwindow* window, float deltaTime);

	};

	extern Camera* mainCamera;
	extern glm::mat4 projectionMatrix;
	extern glm::mat4 viewMatrix;

	void setViewMatrix(unsigned int viewLoc);
	void setProjectionMatrix(float SCREEN_WIDTH, float SCREEN_HEIGHT);
	void updateProjectionMatrix(unsigned int projectionLoc);

	void initializeCamera(GLFWwindow* window,Camera* cam);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


}


#endif