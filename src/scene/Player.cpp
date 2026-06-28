#include "scene/Player.h"
#include "scene/Camera.h"
#include "scene/World.h"

Player::Player(const glm::vec3& spawnPosition)
    : position(spawnPosition)
{
}

void Player::MoveForward(float dt, const glm::vec3& cameraFront)
{
    glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    velocity.x += forward.x * moveSpeed;
    velocity.z += forward.z * moveSpeed;
}

void Player::MoveBackward(float dt, const glm::vec3& cameraFront)
{
    glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    velocity.x -= forward.x * moveSpeed;
    velocity.z -= forward.z * moveSpeed;
}

void Player::MoveLeft(float dt, const glm::vec3& cameraFront)
{
    glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    velocity.x -= right.x * moveSpeed;
    velocity.z -= right.z * moveSpeed;
}

void Player::MoveRight(float dt, const glm::vec3& cameraFront)
{
    glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    velocity.x += right.x * moveSpeed;
    velocity.z += right.z * moveSpeed;
}

void Player::Jump()
{
    if (grounded)
    {
        velocity.y = jumpForce;
        grounded = false;
    }
}

void Player::Update(float dt, const World& world)
{
    velocity.y -= gravity * dt;

    position += velocity * dt;

    ResolveCollisions(world);

    // Damp horizontal velocity each frame so movement is input-driven
    velocity.x = 0.0f;
    velocity.z = 0.0f;
}

void Player::UpdateCamera(Camera& camera) const
{
    camera.position = position + glm::vec3(0.0f, eyeHeight, 0.0f);
}

bool Player::IsGrounded() const
{
    return grounded;
}

AABB Player::GetBoundingBox() const
{
    glm::vec3 center = position + glm::vec3(0.0f, halfExtents.y, 0.0f);
    return AABB::FromCenter(center, halfExtents);
}

void Player::ResolveCollisions(const World& world)
{
    grounded = false;

    auto nearby = world.GetNearbyAABBs(position, 3.0f);

    for (const auto& blockAABB : nearby)
    {
        AABB playerBox = GetBoundingBox();

        if (!playerBox.Overlaps(blockAABB))
            continue;

        glm::vec3 push = playerBox.GetPenetration(blockAABB);
        position += push;

        // Landed on top of a block
        if (push.y > 0.0f)
        {
            velocity.y = 0.0f;
            grounded = true;
        }
        // Hit ceiling
        else if (push.y < 0.0f)
        {
            velocity.y = 0.0f;
        }
    }
}
