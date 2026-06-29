#include "core/Engine.h"
#include "server/HostedServer.h"
#include "game/GameConfig.h"
#include "net/NetworkTypes.h"

#include <cstdio>
#include <cstring>
#include <csignal>
#include <chrono>
#include <thread>

static volatile bool running = true;

static void SignalHandler(int) { running = false; }

static int RunDedicatedServer(int seed, uint16_t port)
{
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    HostedServer server;
    if (!server.Start(seed, GameConfig::ChunkLoadRadius, port))
    {
        std::printf("Failed to start dedicated server\n");
        return 1;
    }

    std::printf("Dedicated server running (seed=%d, port=%d, tick=%.0fHz)\n",
                seed, port, GameConfig::TickRate);
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

    std::printf("\nShutting down...\n");
    server.GetGameServer().SaveAll();
    server.Stop();
    return 0;
}

int main(int argc, char* argv[])
{
    bool dedicated = false;
    int seed = GameConfig::DefaultSeed;
    uint16_t port = Net::DefaultPort;

    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--server") == 0)
            dedicated = true;
        else if (std::strcmp(argv[i], "--seed") == 0 && i + 1 < argc)
            seed = std::atoi(argv[++i]);
        else if (std::strcmp(argv[i], "--port") == 0 && i + 1 < argc)
            port = static_cast<uint16_t>(std::atoi(argv[++i]));
    }

    if (dedicated)
        return RunDedicatedServer(seed, port);

    Engine engine;

    if (!engine.Init("config.json"))
        return -1;

    engine.Run();

    return 0;
}
