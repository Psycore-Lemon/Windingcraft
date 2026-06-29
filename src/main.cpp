#include "core/Engine.h"
#include "server/HostedServer.h"
#include "game/GameConfig.h"
#include "game/SaveData.h"
#include "net/NetworkTypes.h"

#include <cstdio>
#include <cstring>
#include <csignal>
#include <chrono>
#include <thread>
#include <string>

static volatile bool running = true;

static void SignalHandler(int) { running = false; }

static int RunDedicatedServer(const std::string& worldName, int seed, uint16_t port)
{
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    SaveData saveData;
    bool existing = SaveManager::Load(worldName, saveData);

    if (existing)
    {
        seed = saveData.seed;
        std::printf("Loading world '%s' (seed=%d)\n", worldName.c_str(), seed);
    }
    else
    {
        saveData.name = worldName;
        saveData.seed = seed;
        SaveManager::Save(worldName, saveData);
        std::printf("Created world '%s' (seed=%d)\n", worldName.c_str(), seed);
    }

    HostedServer server;
    if (!server.Start(seed, GameConfig::ChunkLoadRadius, port))
    {
        std::printf("Failed to start dedicated server\n");
        return 1;
    }

    std::string worldDir = SaveManager::SaveDir() + "/" + worldName;
    server.SetSaveDir(worldDir);

    std::printf("Server listening on port %d (tick=%.0fHz)\n", port, GameConfig::TickRate);
    std::printf("Press Ctrl+C to stop\n");

    using Clock = std::chrono::steady_clock;
    auto lastTime = Clock::now();
    float accumulator = 0.0f;

    while (running)
    {
        auto now = Clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        accumulator += dt;
        if (accumulator > GameConfig::MaxAccumulator)
            accumulator = GameConfig::MaxAccumulator;

        while (accumulator >= GameConfig::TickInterval)
        {
            server.Tick(GameConfig::TickInterval);
            accumulator -= GameConfig::TickInterval;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::printf("\nSaving world...\n");
    server.GetGameServer().SaveAll();
    std::printf("Shutdown complete.\n");
    server.Stop();
    return 0;
}

int main(int argc, char* argv[])
{
    bool dedicated = false;
    int seed = GameConfig::DefaultSeed;
    uint16_t port = Net::DefaultPort;
    std::string worldName = "server";

    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--server") == 0)
            dedicated = true;
        else if (std::strcmp(argv[i], "--seed") == 0 && i + 1 < argc)
            seed = std::atoi(argv[++i]);
        else if (std::strcmp(argv[i], "--port") == 0 && i + 1 < argc)
            port = static_cast<uint16_t>(std::atoi(argv[++i]));
        else if (std::strcmp(argv[i], "--world") == 0 && i + 1 < argc)
            worldName = argv[++i];
    }

    if (dedicated)
        return RunDedicatedServer(worldName, seed, port);

    Engine engine;

    if (!engine.Init("config.json"))
        return -1;

    engine.Run();

    return 0;
}
