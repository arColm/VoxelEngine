#include "DebugRenderer.h"
#include "Camera.h"


namespace VoxelEngine::DebugRenderer {


	boost::signals2::signal<void()> VoxelEngine::DebugRenderer::drawDebugEvent;

	namespace {
		std::unique_ptr<Shader> debugBlockShader;
	}

	void renderDebug()
	{
		debugBlockShader->use();
		VoxelEngine::mainCamera->updateProjectionMatrixUniform(debugBlockShader->projectionLoc);
		mainCamera->setViewMatrix();
		mainCamera->updateViewMatrixUniform(debugBlockShader->viewLoc);
		debugBlockShader->setVec3("cameraPos", mainCamera->cameraPos);

		VoxelEngine::DebugRenderer::drawDebugEvent();
	}
	void initializeDebugRenderer() {
		debugBlockShader = std::make_unique<Shader>("src/shaders/debugBlockVertexShader.glsl", "src/shaders/debugBlockFragmentShader.glsl");
	}
}