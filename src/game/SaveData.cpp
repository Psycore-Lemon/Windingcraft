#include "game/SaveData.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

std::string SaveManager::SaveDir()
{
    return "saves";
}

void SaveManager::Save(const std::string& saveName, const SaveData& data)
{
    fs::path dir = fs::path(SaveDir()) / saveName;
    fs::create_directories(dir);

    json j;
    j["name"] = data.name;
    j["seed"] = data.seed;
    j["player"]["x"] = data.playerPosition.x;
    j["player"]["y"] = data.playerPosition.y;
    j["player"]["z"] = data.playerPosition.z;

    std::ofstream file(dir / "world.json");
    file << j.dump(4);
}

bool SaveManager::Load(const std::string& saveName, SaveData& data)
{
    fs::path path = fs::path(SaveDir()) / saveName / "world.json";

    if (!fs::exists(path))
        return false;

    std::ifstream file(path);
    json j;
    file >> j;

    data.name = j.value("name", saveName);
    data.seed = j.value("seed", 1337);

    if (j.contains("player"))
    {
        data.playerPosition.x = j["player"].value("x", 0.0f);
        data.playerPosition.y = j["player"].value("y", 20.0f);
        data.playerPosition.z = j["player"].value("z", 0.0f);
    }

    return true;
}

std::vector<std::string> SaveManager::ListSaves()
{
    std::vector<std::string> saves;
    fs::path dir = SaveDir();

    if (!fs::exists(dir))
        return saves;

    for (const auto& entry : fs::directory_iterator(dir))
    {
        if (entry.is_directory() && fs::exists(entry.path() / "world.json"))
            saves.push_back(entry.path().filename().string());
    }

    return saves;
}
