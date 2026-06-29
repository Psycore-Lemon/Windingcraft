#pragma once

#include <glm/glm.hpp>

#include "game/GameConfig.h"
#include "game/Resource.h"
#include "physics/AABB.h"

class Camera;
class InputHandler;
class World;

class Player
{
public:
    Player(const glm::vec3& spawnPosition);

    void ProcessInput(const InputHandler& input, const Camera& camera, float dt);

    void Update(float dt, const World& world);
    void UpdateCamera(Camera& camera) const;

    bool IsGrounded() const;
    bool IsFlying() const;
    const Vitals& GetVitals() const;

    glm::vec3 position;

private:
    AABB GetBoundingBox() const;
    void ResolveCollisions(const World& world, int axis);

    glm::vec3 velocity = glm::vec3(0.0f);

    float moveSpeed = GameConfig::PlayerMoveSpeed;
    float jumpForce = GameConfig::PlayerJumpForce;
    float gravity = GameConfig::PlayerGravity;
    float eyeHeight = GameConfig::PlayerEyeHeight;
    float flySpeed = GameConfig::PlayerFlySpeed;

    glm::vec3 halfExtents = glm::vec3(
        GameConfig::PlayerWidth * 0.5f,
        GameConfig::PlayerHeight * 0.5f,
        GameConfig::PlayerWidth * 0.5f
    );

    Vitals vitals;

    bool grounded = false;
    bool flying = false;

    bool jumpWasDown = false;
    float lastJumpTime = 0.0f;
    float doubleTapWindow = GameConfig::DoubleTapWindow;
    float timeSinceStart = 0.0f;
};
