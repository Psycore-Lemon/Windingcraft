#include "scene/PlayerController.h"
#include "core/InputHandler.h"
#include "game/GameConfig.h"
#include "world/World.h"

#include <cmath>

PlayerController::PlayerController()
    : player(glm::vec3(0.0f, GameConfig::PlayerSpawnY, 0.0f))
{
}

void PlayerController::AttachToWindow(GLFWwindow* window)
{
    camera.AttachToWindow(window);
}

void PlayerController::Reset(const glm::vec3& spawnPosition)
{
    player = Player(spawnPosition);
    inventory = Inventory();
    hasTarget = false;
    breakWasDown = false;
    placeWasDown = false;
}

void PlayerController::ProcessInput(const InputHandler& input, World& world, float dt)
{
    player.ProcessInput(input, camera, dt);
    UpdateLookTarget(world);

    float scroll = input.GetScrollDelta();
    if (scroll != 0.0f)
        inventory.ScrollSelection((int)scroll);

    for (int i = 0; i < GameConfig::HotbarSize; ++i)
    {
        if (input.IsKeyDown(0x31 + i))
            inventory.SetSelectedIndex(i);
    }
    if (input.IsKeyDown(0x30))
        inventory.SetSelectedIndex(GameConfig::HotbarSize);

    bool breakDown = input.IsMouseButtonDown(0);
    if (breakDown && !breakWasDown && hasTarget)
        world.SetBlock(glm::vec3(lookingAtBlockPos), BlockType::Air);
    breakWasDown = breakDown;

    bool placeDown = input.IsMouseButtonDown(1);
    if (placeDown && !placeWasDown && hasTarget)
    {
        BlockType selected = inventory.GetSelectedType();
        if (selected != BlockType::Air)
        {
            glm::vec3 placePos = glm::vec3(lookingAtBlockPos) + glm::vec3(lookingAtNormal);
            world.SetBlock(placePos, selected);
        }
    }
    placeWasDown = placeDown;
}

void PlayerController::Update(float dt, World& world)
{
    world.Update(player.position);
    player.Update(dt, world);
    player.UpdateCamera(camera);
}

Camera& PlayerController::GetCamera() { return camera; }
const Camera& PlayerController::GetCamera() const { return camera; }
const Player& PlayerController::GetPlayer() const { return player; }
const Inventory& PlayerController::GetInventory() const { return inventory; }
const glm::ivec3& PlayerController::GetLookingAtPos() const { return lookingAtBlockPos; }
bool PlayerController::HasTarget() const { return hasTarget; }

PlayerStatus PlayerController::GetStatus() const
{
    PlayerStatus status;
    status.position = player.position;
    status.grounded = player.IsGrounded();
    status.flying = player.IsFlying();
    status.hasTarget = hasTarget;
    status.lookingAtBlock = lookingAtBlock;
    return status;
}

void PlayerController::UpdateLookTarget(const World& world)
{
    hasTarget = false;
    lookingAtBlock = BlockType::Air;
    lookingAtNormal = glm::ivec3(0);

    glm::vec3 pos = camera.position;
    glm::vec3 dir = camera.GetFront();
    glm::ivec3 prevBlock(
        (int)std::floor(pos.x),
        (int)std::floor(pos.y),
        (int)std::floor(pos.z)
    );

    for (float t = 0.0f; t < GameConfig::BlockReach; t += GameConfig::RayStep)
    {
        glm::vec3 point = pos + dir * t;
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
