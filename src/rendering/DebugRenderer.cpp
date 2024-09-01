#include "DebugRenderer.h"
#include "Camera.h"
#include "loader.h"


namespace VoxelEngine::DebugRenderer {


	boost::signals2::signal<void()> VoxelEngine::DebugRenderer::drawDebugEvent;

	namespace {
		std::unique_ptr<Shader> debugBlockShader;
		GLuint debugVAO;
		std::vector<GLfloat> cubeVertices;
	}

	void renderDebug()
	{
		debugBlockShader->use();
		VoxelEngine::mainCamera->updateProjectionMatrixUniform(debugBlockShader->projectionLoc);
		mainCamera->setViewMatrix();
		mainCamera->updateViewMatrixUniform(debugBlockShader->viewLoc);
		debugBlockShader->setVec3("cameraPos", mainCamera->cameraPos);

		VoxelEngine::DebugRenderer::drawDebugEvent();

		//render VAO
		glBindVertexArray(debugVAO);
		GLuint positionVBO = Loader::createVBO();
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(cubeVertices.at(0)), cubeVertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size());

		cubeVertices.clear();

		glBindVertexArray(0);
		glDeleteBuffers(1, &positionVBO);
	}
	void initializeDebugRenderer() {
		debugBlockShader = std::make_unique<Shader>("src/shaders/debugBlockVertexShader.glsl", "src/shaders/debugBlockFragmentShader.glsl");
		debugVAO = VoxelEngine::Loader::createVAO();
	}
	void releaseResources() {
		glDeleteBuffers(1, &debugVAO);
	}
	void drawCube(glm::vec3 pos, glm::vec3 color)
	{
		std::array<GLfloat,24> vertices = {
			pos.x-0.5f,pos.y+0.5f,pos.z-0.5f,
			pos.x-0.5f,pos.y+0.5f,pos.z+0.5f,
			pos.x+0.5f,pos.y+0.5f,pos.z+0.5f,
			pos.x+0.5f,pos.y+0.5f,pos.z+0.5f,
			pos.x+0.5f,pos.y+0.5f,pos.z-0.5f,
			pos.x-0.5f,pos.y+0.5f,pos.z-0.5f,
		};
		cubeVertices.insert(cubeVertices.end(), vertices.begin(), vertices.end());
	}
}