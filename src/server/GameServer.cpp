#include "server/GameServer.h"
#include "scene/Player.h"
#include "world/World.h"

GameServer::GameServer() = default;
GameServer::~GameServer() = default;

void GameServer::Init(int seed, int loadRadius)
{
    world = std::make_unique<World>(seed, loadRadius);
}

void GameServer::SetSaveDir(const std::string& dir)
{
    world->SetSaveDir(dir);
}

int GameServer::AddPlayer(const glm::vec3& spawnPos)
{
    int id = nextPlayerId++;
    players[id] = std::make_unique<Player>(spawnPos);

    world->Update(spawnPos);
    return id;
}

void GameServer::RemovePlayer(int id)
{
    players.erase(id);
    pendingCommands.erase(id);
}

void GameServer::QueueCommand(int playerId, const PlayerCommand& cmd)
{
    pendingCommands[playerId] = cmd;
}

void GameServer::Tick(float dt)
{
    for (auto& [id, player] : players)
    {
        auto it = pendingCommands.find(id);
        if (it != pendingCommands.end())
            player->ProcessCommand(it->second, dt, *world);

        player->PhysicsTick(dt, *world);
    }

    for (auto& [id, player] : players)
        world->Update(player->position);
}

Player& GameServer::GetPlayer(int id) { return *players.at(id); }
const Player& GameServer::GetPlayer(int id) const { return *players.at(id); }
const std::unordered_map<int, std::unique_ptr<Player>>& GameServer::GetPlayers() const { return players; }

World& GameServer::GetWorld() { return *world; }
const World& GameServer::GetWorld() const { return *world; }

void GameServer::SaveAll()
{
    world->SaveAll();
}
