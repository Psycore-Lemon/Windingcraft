#include "ui/VideoSettings.h"
#include "core/Window.h"
#include "core/Config.h"

#include <imgui.h>

struct Resolution { int w; int h; const char* label; };

static const Resolution resolutions[] = {
    { 1280,  720, "1280 x 720"   },
    { 1600,  900, "1600 x 900"   },
    { 1920, 1080, "1920 x 1080"  },
    { 2560, 1440, "2560 x 1440"  },
    { 3440, 1440, "3440 x 1440"  },
};

void VideoSettings::Render(Window& window, Config& config, const std::string& configPath)
{
    ImGui::Text("Video");
    ImGui::Spacing();

    for (const auto& res : resolutions)
    {
        if (ImGui::Button(res.label, ImVec2(200, 30)))
        {
            if (window.IsFullscreen())
                window.SetFullscreen(false);
            window.SetSize(res.w, res.h);

            config.windowWidth = res.w;
            config.windowHeight = res.h;
            config.fullscreen = false;
            WriteConfig(configPath, config);
        }
    }

    ImGui::Spacing();

    bool fullscreen = window.IsFullscreen();
    if (ImGui::Checkbox("Fullscreen", &fullscreen))
    {
        window.SetFullscreen(fullscreen);
        config.fullscreen = fullscreen;
        WriteConfig(configPath, config);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Camera");
    ImGui::Spacing();

    if (ImGui::SliderFloat("FOV", &config.fov, 30.0f, 120.0f, "%.0f"))
    {
        WriteConfig(configPath, config);
    }
}
