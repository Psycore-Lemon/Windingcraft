#pragma once

namespace GameConfig
{
    // Player
    constexpr float PlayerMoveSpeed = 5.0f;
    constexpr float PlayerFlySpeed = 10.0f;
    constexpr float PlayerJumpForce = 7.0f;
    constexpr float PlayerGravity = 20.0f;
    constexpr float PlayerEyeHeight = 1.6f;
    constexpr float PlayerWidth = 0.6f;
    constexpr float PlayerHeight = 1.8f;
    constexpr float DoubleTapWindow = 0.3f;
    constexpr float MaxDeltaTime = 0.05f;
    constexpr float TickRate = 30.0f;
    constexpr float TickInterval = 1.0f / TickRate;
    constexpr float MaxAccumulator = 0.25f;

    // World
    constexpr int ChunkSize = 16;
    constexpr int ChunkMaxHeight = 64;
    constexpr int ChunkLoadRadius = 8;
    constexpr int DefaultSeed = 1337;

    // Terrain
    constexpr int TerrainBaseHeight = 10;
    constexpr int TerrainHeightRange = 8;
    constexpr int TerrainDirtDepth = 3;
    constexpr float TerrainNoiseFrequency = 0.02f;
    constexpr int TerrainNoiseOctaves = 4;

    // Vitals
    constexpr float BaseHealth = 100.0f;
    constexpr float BaseMana = 50.0f;
    constexpr float BaseStamina = 100.0f;
    constexpr float StaminaRegenRate = 15.0f;
    constexpr float ManaRegenRate = 3.0f;

    // Interaction
    constexpr float BlockReach = 8.0f;
    constexpr float RayStep = 0.05f;
    constexpr float PlayerSpawnY = 20.0f;

    // Inventory
    constexpr int HotbarSize = 9;
    constexpr int DefaultStackSize = 64;

    // Camera
    constexpr float CameraFOV = 45.0f;
    constexpr float CameraNearPlane = 0.1f;
    constexpr float CameraFarPlane = 1000.0f;
    constexpr float MouseSensitivity = 0.1f;
}
