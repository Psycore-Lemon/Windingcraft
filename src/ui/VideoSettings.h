#pragma once

#include <string>

class Window;
struct Config;

class VideoSettings
{
public:
    static void Render(Window& window, Config& config, const std::string& configPath);
};
