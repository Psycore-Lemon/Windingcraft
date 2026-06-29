#pragma once

#include <glm/glm.hpp>

#include "game/PlayerCommand.h"
#include "scene/Camera.h"
#include "scene/PlayerStatus.h"

class InputHandler;
class Player;
struct GLFWwindow;

class PlayerController
{
public:
    void AttachToWindow(GLFWwindow* window);

    PlayerCommand BuildCommand(const InputHandler& input) const;
    void SyncFromServer(const Player& serverPlayer);

    PlayerStatus GetStatus(const Player& serverPlayer) const;

    Camera& GetCamera();
    const Camera& GetCamera() const;

private:
    Camera camera;
};
