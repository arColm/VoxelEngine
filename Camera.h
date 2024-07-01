#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>

namespace VoxelEngine::Camera {
	/*===============================
		DEFAULT CAMERA
	=================================*/

	void setViewMatrix(unsigned int viewLoc);
	void setProjectionMatrix(unsigned int projectionLoc, float SCREEN_WIDTH, float SCREEN_HEIGHT);
	void initializeCamera(GLFWwindow* window);
	void moveCamera(GLFWwindow* window, float deltaTime);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
}


#endif