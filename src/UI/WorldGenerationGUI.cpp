
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include<src/UI/WorldGenerationGUI.h>

namespace VoxelEngine::GUI {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	void ShowWorldGenerationGUIWindow(World* world) {
		IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context.");
		IMGUI_CHECKVERSION();

        static float f = 0.0f;
        static int counter = 0;
        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::SetWindowPos(ImVec2(800, 0));
        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Reload Chunks"))
            world->reloadChunks();

        ImGui::End();
		ShowTest();
	}
	
	void ShowTest() {

	}
}