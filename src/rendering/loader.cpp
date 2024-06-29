

#include <src/rendering/loader.h>



namespace VoxelEngine {

	void Loader::DrawVertexData(std::vector<GLfloat> positions, std::vector<GLfloat> colors) {
		unsigned int VAO = bindVAO();
		unsigned int VBOpos = bindVBO();


		unsigned int VBOcolor = bindVBO();


		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBOpos);
		glDeleteBuffers(1, &VBOcolor);
	}
	
	GLuint Loader::createVAO() {
		unsigned int VAO;
		glGenVertexArrays(1, &VAO);
		return VAO;
	}

	GLuint Loader::createVBO() {
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		return VBO;
	}

	namespace {
		GLuint bindVBO() {
			unsigned int VBO;
			glGenBuffers(1, &VBO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			return VBO;
		}

		GLuint bindVAO() {
			unsigned int VAO;
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			return VAO;
		}
	}
}