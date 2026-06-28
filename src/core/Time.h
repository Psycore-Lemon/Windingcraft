#pragma once
#include <GLFW/glfw3.h>

class Time
{
public:
    void Update()
    {
        double current = glfwGetTime();
        deltaTime = current - lastTime;
        lastTime = current;
    }

    double DeltaTime() const { return deltaTime; }

private:
    double lastTime = 0.0;
    double deltaTime = 0.0;
};