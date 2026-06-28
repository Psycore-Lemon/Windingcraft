#pragma once

#include <glm/glm.hpp>

#include "physics/AABB.h"

class Camera;
class World;

class Player
{
public:
    Player(const glm::vec3& spawnPosition);

    void MoveForward(float dt, const glm::vec3& cameraFront);
    void MoveBackward(float dt, const glm::vec3& cameraFront);
    void MoveLeft(float dt, const glm::vec3& cameraFront);
    void MoveRight(float dt, const glm::vec3& cameraFront);
    void Jump();

    void Update(float dt, const World& world);
    void UpdateCamera(Camera& camera) const;

    bool IsGrounded() const;

    glm::vec3 position;

private:
    AABB GetBoundingBox() const;
    void ResolveCollisions(const World& world);

    glm::vec3 velocity = glm::vec3(0.0f);

    float moveSpeed = 5.0f;
    float jumpForce = 7.0f;
    float gravity = 20.0f;
    float eyeHeight = 1.6f;

    // Player hitbox: 0.6 wide, 1.8 tall, 0.6 deep
    glm::vec3 halfExtents = glm::vec3(0.3f, 0.9f, 0.3f);

    bool grounded = false;
};
