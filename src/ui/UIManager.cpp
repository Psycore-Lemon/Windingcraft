#include "ui/UIManager.h"
#include "scene/PlayerStatus.h"
#include "game/Inventory.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

UIManager::~UIManager()
{
    if (initialized)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

void UIManager::Init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    this->window = window;
    initialized = true;
}

void UIManager::BeginFrame(bool interactive)
{
    ImGuiIO& io = ImGui::GetIO();

    if (interactive)
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    else
        io.ConfigFlags |= ImGuiConfigFlags_NoMouse;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

MainMenu::Action UIManager::RenderMainMenu(Window& window, Config& config, const std::string& configPath)
{
    return mainMenu.Render(window, config, configPath);
}

PauseMenu::Action UIManager::RenderPaused(Window& window, Config& config, const std::string& configPath,
                                           const PlayerStatus& status, const Inventory& inventory, int chunkCount)
{
    hud.RenderOverlay(status, chunkCount);

    return pauseMenu.Render(window, config, configPath);
}

void UIManager::RenderPlaying(const PlayerStatus& status, const Inventory& inventory, int chunkCount)
{
    hud.RenderOverlay(status, chunkCount);
    hud.RenderResourceBars(status);
    hud.RenderCrosshair();
    hotbar.Render(inventory);
}

MainMenu& UIManager::GetMainMenu() { return mainMenu; }
PauseMenu& UIManager::GetPauseMenu() { return pauseMenu; }
