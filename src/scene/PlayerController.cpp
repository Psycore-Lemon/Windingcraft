#include "scene/PlayerController.h"
#include "scene/Player.h"
#include "core/InputHandler.h"
#include "game/GameConfig.h"

#include <GLFW/glfw3.h>

void PlayerController::AttachToWindow(GLFWwindow* window)
{
    camera.AttachToWindow(window);
}

PlayerCommand PlayerController::BuildCommand(const InputHandler& input) const
{
    PlayerCommand cmd;

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

    cmd.moveDirection = move;
    cmd.lookDirection = front;
    cmd.jump = input.IsActionDown(Action::Jump);
    cmd.descend = input.IsActionDown(Action::Descend);
    cmd.primaryAction = input.IsMouseButtonDown(0);
    cmd.secondaryAction = input.IsMouseButtonDown(1);
    cmd.scrollDelta = input.GetScrollDelta();

    for (int i = 0; i < GameConfig::HotbarSize; ++i)
    {
        if (input.IsKeyDown(0x31 + i))
        {
            cmd.hotbarSelect = i;
            break;
        }
    }
    if (cmd.hotbarSelect < 0 && input.IsKeyDown(0x30))
        cmd.hotbarSelect = GameConfig::HotbarSize;

    return cmd;
}

void PlayerController::SyncFromServer(const Player& serverPlayer)
{
    camera.position = serverPlayer.GetEyePosition();
}

PlayerStatus PlayerController::GetStatus(const Player& serverPlayer) const
{
    PlayerStatus status;
    status.position = serverPlayer.position;
    status.grounded = serverPlayer.IsGrounded();
    status.flying = serverPlayer.IsFlying();
    status.hasTarget = serverPlayer.HasTarget();
    status.lookingAtBlock = serverPlayer.GetTargetBlock();
    status.vitals = serverPlayer.GetVitals();
    return status;
}

Camera& PlayerController::GetCamera() { return camera; }
const Camera& PlayerController::GetCamera() const { return camera; }
