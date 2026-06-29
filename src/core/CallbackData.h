#pragma once

class Camera;
class InputHandler;

struct CallbackData
{
    Camera* camera = nullptr;
    InputHandler* input = nullptr;
};
