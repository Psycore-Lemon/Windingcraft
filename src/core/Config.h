#pragma once

#include <string>


struct Config
{
	unsigned int windowWidth = 1280;
	unsigned int windowHeight = 720;
	bool fullscreen = false;
};

Config* ReadConfig(const std::string& filepath);

void WriteConfig(const std::string& filepath, const Config* config);
