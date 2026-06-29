#pragma once

#include <glm/glm.hpp>

#include "game/BlockType.h"
#include "game/Inventory.h"
#include "scene/Camera.h"
#include "scene/Player.h"
#include "scene/PlayerStatus.h"

class InputHandler;
class World;
struct GLFWwindow;

class PlayerController
{
public:
    PlayerController();

    void AttachToWindow(GLFWwindow* window);
    void Reset(const glm::vec3& spawnPosition);

    void ProcessInput(const InputHandler& input, World& world, float dt);
    void Update(float dt, World& world);

    Camera& GetCamera();
    const Camera& GetCamera() const;
    const Player& GetPlayer() const;
    const Inventory& GetInventory() const;

    PlayerStatus GetStatus() const;
    const glm::ivec3& GetLookingAtPos() const;
    bool HasTarget() const;

private:
    void UpdateLookTarget(const World& world);

    Camera camera;
    Player player;
    Inventory inventory;

    BlockType lookingAtBlock = BlockType::Air;
    glm::ivec3 lookingAtBlockPos = glm::ivec3(0);
    glm::ivec3 lookingAtNormal = glm::ivec3(0);
    bool hasTarget = false;

    bool breakWasDown = false;
    bool placeWasDown = false;
};
