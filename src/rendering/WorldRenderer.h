#pragma once
#include <src/World/world.h>


namespace VoxelEngine {

	class WorldRenderer {
	public:
		static const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
		inline static const float sunMaxHeight = 400.f;
		static float SCREEN_WIDTH;
		static float SCREEN_HEIGHT;

		WorldRenderer(std::shared_ptr<World> world);
		~WorldRenderer();

		void renderFrame(); //does full render of 1 frame
		void renderScene(); //renders scene once (does 1 render pass)

		std::shared_ptr<World> world;
	private:
		std::shared_ptr<Shader> defaultBlockShader;
		std::shared_ptr<Shader> shadowMapShader;
		std::unique_ptr<Shader> skyboxShader;
		std::unique_ptr<Shader> sunMoonShader;
		glm::vec3 sunPos;
		glm::vec3 sunLightDirection;


		void generateMeshes();
		glm::mat4 generateLightSpaceMatrix();

		void renderSkybox();

		unsigned int depthMapFBO;
		unsigned int depthMap;

		unsigned int skyboxVAO;
		unsigned int sunMoonVAO;


		boost::signals2::connection debugRendererConnection;
		void onDrawDebug();

	};




}