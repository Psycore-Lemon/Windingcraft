#include "scene/Player.h"
#include "world/World.h"

#include <cmath>

Player::Player(const glm::vec3& spawnPosition)
    : position(spawnPosition)
    , vitals{
        { GameConfig::BaseHealth, GameConfig::BaseHealth },
        { GameConfig::BaseMana, GameConfig::BaseMana },
        { GameConfig::BaseStamina, GameConfig::BaseStamina }
    }
{
}

void Player::ProcessCommand(const PlayerCommand& cmd, float dt, World& world)
{
    timeSinceStart += dt;
    lookDirection = cmd.lookDirection;

    float speed = flying ? flySpeed : moveSpeed;
    velocity.x = cmd.moveDirection.x * speed;
    velocity.z = cmd.moveDirection.z * speed;

    if (cmd.jump && !jumpWasDown)
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
    jumpWasDown = cmd.jump;

    if (flying)
    {
        velocity.y = 0.0f;
        if (cmd.jump)    velocity.y = flySpeed;
        if (cmd.descend) velocity.y = -flySpeed;
    }

    // Hotbar
    if (cmd.scrollDelta != 0.0f)
        inventory.ScrollSelection((int)cmd.scrollDelta);

    if (cmd.hotbarSelect >= 0)
        inventory.SetSelectedIndex(cmd.hotbarSelect);

    // Raycast
    UpdateTarget(world);

    // Block actions (edge-triggered)
    if (cmd.primaryAction && !primaryWasDown && hasTarget)
        world.SetBlock(glm::vec3(lookingAtBlockPos), BlockType::Air);
    primaryWasDown = cmd.primaryAction;

    if (cmd.secondaryAction && !secondaryWasDown && hasTarget)
    {
        BlockType selected = inventory.GetSelectedType();
        if (selected != BlockType::Air)
        {
            glm::vec3 placePos = glm::vec3(lookingAtBlockPos) + glm::vec3(lookingAtNormal);
            world.SetBlock(placePos, selected);
        }
    }
    secondaryWasDown = cmd.secondaryAction;
}

void Player::PhysicsTick(float dt, const World& world)
{
    if (!flying)
        velocity.y -= gravity * dt;

    position.y += velocity.y * dt;
    ResolveCollisions(world, 1);

    position.x += velocity.x * dt;
    ResolveCollisions(world, 0);

    position.z += velocity.z * dt;
    ResolveCollisions(world, 2);

    vitals.stamina.Restore(GameConfig::StaminaRegenRate * dt);
    vitals.mana.Restore(GameConfig::ManaRegenRate * dt);
}

bool Player::IsGrounded() const { return grounded; }
bool Player::IsFlying() const { return flying; }
const Vitals& Player::GetVitals() const { return vitals; }
Inventory& Player::GetInventory() { return inventory; }
const Inventory& Player::GetInventory() const { return inventory; }

bool Player::HasTarget() const { return hasTarget; }
BlockType Player::GetTargetBlock() const { return lookingAtBlock; }
const glm::ivec3& Player::GetTargetBlockPos() const { return lookingAtBlockPos; }

glm::vec3 Player::GetEyePosition() const
{
    return position + glm::vec3(0.0f, eyeHeight, 0.0f);
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

void Player::UpdateTarget(const World& world)
{
    hasTarget = false;
    lookingAtBlock = BlockType::Air;
    lookingAtNormal = glm::ivec3(0);

    glm::vec3 origin = GetEyePosition();
    glm::ivec3 prevBlock(
        (int)std::floor(origin.x),
        (int)std::floor(origin.y),
        (int)std::floor(origin.z)
    );

    for (float t = 0.0f; t < GameConfig::BlockReach; t += GameConfig::RayStep)
    {
        glm::vec3 point = origin + lookDirection * t;
        glm::ivec3 block(
            (int)std::floor(point.x),
            (int)std::floor(point.y),
            (int)std::floor(point.z)
        );

        BlockType type = world.GetBlock(point);

        if (Blocks::IsSolid(type))
        {
            hasTarget = true;
            lookingAtBlock = type;
            lookingAtBlockPos = block;
            lookingAtNormal = prevBlock - block;
            return;
        }

        prevBlock = block;
    }
}
