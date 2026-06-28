#include "graphics/HUD.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

HUD::~HUD()
{
    if (initialized)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

void HUD::Init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 330");

    this->window = window;
    initialized = true;
}

void HUD::BeginFrame(bool interactive)
{
    ImGuiIO& io = ImGui::GetIO();

    if (interactive)
    {
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        io.AddMousePosEvent((float)mx, (float)my);
        io.AddMouseButtonEvent(0, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    }
    else
    {
        io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void HUD::RenderOverlay(const glm::vec3& playerPos, bool grounded, int chunkCount)
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.5f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
                           | ImGuiWindowFlags_AlwaysAutoResize
                           | ImGuiWindowFlags_NoFocusOnAppearing
                           | ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("HUD", nullptr, flags))
    {
        ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
        ImGui::Separator();
        ImGui::Text("Pos: %.1f, %.1f, %.1f", playerPos.x, playerPos.y, playerPos.z);
        ImGui::Text("Grounded: %s", grounded ? "Yes" : "No");
        ImGui::Text("Chunks: %d", chunkCount);
    }
    ImGui::End();
}

void HUD::RenderCrosshair()
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    float size = 10.0f;
    float thickness = 2.0f;
    ImU32 color = IM_COL32(255, 255, 255, 200);

    ImDrawList* draw = ImGui::GetForegroundDrawList();
    draw->AddLine(ImVec2(center.x - size, center.y), ImVec2(center.x + size, center.y), color, thickness);
    draw->AddLine(ImVec2(center.x, center.y - size), ImVec2(center.x, center.y + size), color, thickness);
}

HUD::PauseAction HUD::RenderPauseMenu()
{
    PauseAction action = PauseAction::None;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowBgAlpha(0.85f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
                           | ImGuiWindowFlags_AlwaysAutoResize
                           | ImGuiWindowFlags_NoMove;

    if (ImGui::Begin("Pause", nullptr, flags))
    {
        ImGui::Text("PAUSED");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Resume", ImVec2(200, 40)))
            action = PauseAction::Resume;

        ImGui::Spacing();

        if (ImGui::Button("Quit", ImVec2(200, 40)))
            action = PauseAction::Quit;
    }
    ImGui::End();

    return action;
}

void HUD::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
