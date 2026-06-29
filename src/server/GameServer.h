#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

#include "game/PlayerCommand.h"

class Player;
class World;

class GameServer
{
public:
    GameServer();
    ~GameServer();

    void Init(int seed, int loadRadius);
    void SetSaveDir(const std::string& dir);

    int AddPlayer(const glm::vec3& spawnPos);
    void RemovePlayer(int id);

    void QueueCommand(int playerId, const PlayerCommand& cmd);
    void Tick(float dt);

    Player& GetPlayer(int id);
    const Player& GetPlayer(int id) const;

    World& GetWorld();
    const World& GetWorld() const;

    void SaveAll();

private:
    std::unique_ptr<World> world;
    std::unordered_map<int, std::unique_ptr<Player>> players;
    std::unordered_map<int, PlayerCommand> pendingCommands;
    int nextPlayerId = 0;
};
