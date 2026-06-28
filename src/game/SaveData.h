#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct SaveData
{
    std::string name;
    int seed = 1337;
    glm::vec3 playerPosition = glm::vec3(0.0f, 20.0f, 0.0f);
};

class SaveManager
{
public:
    static void Save(const std::string& saveName, const SaveData& data);
    static bool Load(const std::string& saveName, SaveData& data);
    static void Delete(const std::string& saveName);
    static std::vector<std::string> ListSaves();
    static std::string SaveDir();
};
