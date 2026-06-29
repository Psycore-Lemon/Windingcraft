#include "Config.h"

#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Config ReadConfig(const std::string& filepath)
{
    Config config;

    if (!std::filesystem::exists(filepath))
    {
        WriteConfig(filepath, config);
        return config;
    }

    std::ifstream inFile(filepath);
    json data;
    inFile >> data;

    config.windowWidth = data["window"].value("width", config.windowWidth);
    config.windowHeight = data["window"].value("height", config.windowHeight);
    config.fullscreen = data["window"].value("fullscreen", config.fullscreen);
    config.fov = data.value("fov", config.fov);

    return config;
}

void WriteConfig(const std::string& filepath, const Config& config)
{
    json data;
    data["window"]["width"] = config.windowWidth;
    data["window"]["height"] = config.windowHeight;
    data["window"]["fullscreen"] = config.fullscreen;
    data["fov"] = config.fov;

    std::ofstream file(filepath);
    file << data.dump(4);
}
