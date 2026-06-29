#pragma once

#include <string>

struct Config
{
	unsigned int windowWidth = 1280;
	unsigned int windowHeight = 720;
	bool fullscreen = false;
	float fov = 45.0f;
};

Config ReadConfig(const std::string& filepath);
void WriteConfig(const std::string& filepath, const Config& config);
