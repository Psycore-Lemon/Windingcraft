#pragma once

#include <glm/glm.hpp>

#include "game/GameConfig.h"
#include "game/Resource.h"
#include "game/Inventory.h"
#include "game/PlayerCommand.h"
#include "physics/AABB.h"

class World;

class Player
{
public:
    Player(const glm::vec3& spawnPosition);

    void ProcessCommand(const PlayerCommand& cmd, float dt, World& world);
    void PhysicsTick(float dt, const World& world);

    bool IsGrounded() const;
    bool IsFlying() const;
    const Vitals& GetVitals() const;

    Inventory& GetInventory();
    const Inventory& GetInventory() const;

    bool HasTarget() const;
    BlockType GetTargetBlock() const;
    const glm::ivec3& GetTargetBlockPos() const;

    glm::vec3 GetEyePosition() const;

    glm::vec3 position;

private:
    AABB GetBoundingBox() const;
    void ResolveCollisions(const World& world, int axis);
    void UpdateTarget(const World& world);

    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 lookDirection = glm::vec3(0.0f, 0.0f, -1.0f);

    Vitals vitals;
    Inventory inventory;

    bool hasTarget = false;
    BlockType lookingAtBlock = BlockType::Air;
    glm::ivec3 lookingAtBlockPos = glm::ivec3(0);
    glm::ivec3 lookingAtNormal = glm::ivec3(0);

    bool primaryWasDown = false;
    bool secondaryWasDown = false;
    bool jumpWasDown = false;
    float lastJumpTime = 0.0f;
    float timeSinceStart = 0.0f;

    float moveSpeed = GameConfig::PlayerMoveSpeed;
    float jumpForce = GameConfig::PlayerJumpForce;
    float gravity = GameConfig::PlayerGravity;
    float eyeHeight = GameConfig::PlayerEyeHeight;
    float flySpeed = GameConfig::PlayerFlySpeed;
    float doubleTapWindow = GameConfig::DoubleTapWindow;

    glm::vec3 halfExtents = glm::vec3(
        GameConfig::PlayerWidth * 0.5f,
        GameConfig::PlayerHeight * 0.5f,
        GameConfig::PlayerWidth * 0.5f
    );

    bool grounded = false;
    bool flying = false;
};
