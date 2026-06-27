#include "Config.h"

#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;


Config* ReadConfig(const std::string& filepath)
{
	Config* config = new Config;

	if (!std::filesystem::exists(filepath))
	{
        WriteConfig(filepath, config);
        return config;
	}

    std::ifstream inFile(filepath);

    json data;
    inFile >> data;

    // Override defaults
    config->windowWidth = data["window"].value("width", config->windowWidth);

    config->windowHeight = data["window"].value("height", config->windowHeight);

    config->fullscreen = data["window"].value("title", config->fullscreen);


    return config;

}

void WriteConfig(const std::string& filepath,const Config* config)
{
    json data;

    data["window"]["width"] = config->windowWidth;

    data["window"]["height"] = config->windowHeight;

    data["window"]["fullscreen"] = config->fullscreen;

    std::ofstream file(filepath);

    file << data.dump(4);
}