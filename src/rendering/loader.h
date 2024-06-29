#ifndef LOADER_H
#define LOADER_H

#include <vector>
#include <glad/glad.h>

namespace VoxelEngine {
	namespace Loader {
		void DrawVertexData(std::vector<GLfloat> positions, std::vector<GLfloat> colors);
		GLuint createVAO();
		GLuint createVBO();
	}

	namespace {
		GLuint bindVBO();
		GLuint bindVAO();
	}
}


#endif