#include "InputHandler.h"
#include "core/CallbackData.h"
#include <GLFW/glfw3.h>

InputHandler::InputHandler(GLFWwindow* window)
    : window(window)
{
    BindKey(Action::MoveForward,  GLFW_KEY_W);
    BindKey(Action::MoveBackward, GLFW_KEY_S);
    BindKey(Action::MoveLeft,     GLFW_KEY_A);
    BindKey(Action::MoveRight,    GLFW_KEY_D);
    BindKey(Action::Jump,         GLFW_KEY_SPACE);
    BindKey(Action::Descend,      GLFW_KEY_LEFT_SHIFT);
    BindKey(Action::Pause,        GLFW_KEY_ESCAPE);

    double x;
    double y;
    glfwGetCursorPos(window, &x, &y);

    currentMousePosition = glm::vec2(x, y);
    lastMousePosition = currentMousePosition;

    glfwSetScrollCallback(window, ScrollCallback);
}

void InputHandler::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* data = static_cast<CallbackData*>(glfwGetWindowUserPointer(window));
    if (data && data->input)
        data->input->scrollDelta += (float)yoffset;
}

void InputHandler::Update()
{
    scrollDelta = 0.0f;
    glfwPollEvents();

    double x;
    double y;
    glfwGetCursorPos(window, &x, &y);

    currentMousePosition = glm::vec2(x, y);

    if (firstMouseUpdate)
    {
        lastMousePosition = currentMousePosition;
        mouseDelta = glm::vec2(0.0f);
        firstMouseUpdate = false;
    }
    else
    {
        mouseDelta = currentMousePosition - lastMousePosition;
        lastMousePosition = currentMousePosition;
    }
}

void InputHandler::BindKey(Action action, int key)
{
    keyBindings[action] = key;
}

bool InputHandler::IsKeyDown(int key) const
{
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool InputHandler::IsActionDown(Action action) const
{
    auto it = keyBindings.find(action);

    if (it == keyBindings.end())
    {
        return false;
    }

    int key = it->second;
    return IsKeyDown(key);
}

bool InputHandler::IsMouseButtonDown(int button) const
{
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

glm::vec2 InputHandler::GetMousePosition() const
{
    return currentMousePosition;
}

glm::vec2 InputHandler::GetMouseDelta() const
{
    return mouseDelta;
}

float InputHandler::GetScrollDelta() const
{
    return scrollDelta;
}