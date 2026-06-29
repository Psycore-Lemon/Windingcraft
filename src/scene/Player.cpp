#include "scene/Player.h"
#include "scene/Camera.h"
#include "world/World.h"
#include "core/InputHandler.h"

Player::Player(const glm::vec3& spawnPosition)
    : position(spawnPosition)
{
}

void Player::ProcessInput(const InputHandler& input, const Camera& camera, float dt)
{
    timeSinceStart += dt;

    glm::vec3 front = camera.GetFront();
    glm::vec3 forward = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 move(0.0f);

    if (input.IsActionDown(Action::MoveForward))  move += forward;
    if (input.IsActionDown(Action::MoveBackward)) move -= forward;
    if (input.IsActionDown(Action::MoveLeft))     move -= right;
    if (input.IsActionDown(Action::MoveRight))    move += right;

    if (glm::length(move) > 0.0f)
        move = glm::normalize(move);

    float speed = flying ? flySpeed : moveSpeed;
    velocity.x = move.x * speed;
    velocity.z = move.z * speed;

    bool jumpDown = input.IsActionDown(Action::Jump);

    if (jumpDown && !jumpWasDown)
    {
        if (timeSinceStart - lastJumpTime < doubleTapWindow)
            flying = !flying;

        lastJumpTime = timeSinceStart;

        if (!flying && grounded)
        {
            velocity.y = jumpForce;
            grounded = false;
        }
    }
    jumpWasDown = jumpDown;

    if (flying)
    {
        velocity.y = 0.0f;

        if (input.IsActionDown(Action::Jump))
            velocity.y = flySpeed;

        if (input.IsActionDown(Action::Descend))
            velocity.y = -flySpeed;
    }
}

void Player::Update(float dt, const World& world)
{
    if (!flying)
        velocity.y -= gravity * dt;

    position.y += velocity.y * dt;
    ResolveCollisions(world, 1);

    position.x += velocity.x * dt;
    ResolveCollisions(world, 0);

    position.z += velocity.z * dt;
    ResolveCollisions(world, 2);
}

void Player::UpdateCamera(Camera& camera) const
{
    camera.position = position + glm::vec3(0.0f, eyeHeight, 0.0f);
}

bool Player::IsGrounded() const
{
    return grounded;
}

bool Player::IsFlying() const
{
    return flying;
}

AABB Player::GetBoundingBox() const
{
    glm::vec3 center = position + glm::vec3(0.0f, halfExtents.y, 0.0f);
    return AABB::FromCenter(center, halfExtents);
}

void Player::ResolveCollisions(const World& world, int axis)
{
    if (axis == 1)
        grounded = false;

    auto nearby = world.GetNearbyAABBs(position, 3.0f);

    for (const auto& blockAABB : nearby)
    {
        AABB playerBox = GetBoundingBox();

        if (!playerBox.Overlaps(blockAABB))
            continue;

        glm::vec3 push = playerBox.GetPenetrationAxis(blockAABB, axis);
        position += push;

        if (axis == 1)
        {
            if (push.y > 0.0f)
            {
                velocity.y = 0.0f;
                grounded = true;
            }
            else if (push.y < 0.0f)
            {
                velocity.y = 0.0f;
            }
        }
    }
}
