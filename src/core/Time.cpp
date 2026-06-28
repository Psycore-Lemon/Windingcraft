#include "core/Time.h"
#include <GLFW/glfw3.h>

void Time::Update()
{
    double current = glfwGetTime();
    deltaTime = current - lastTime;
    lastTime = current;
}

double Time::DeltaTime() const
{
    return deltaTime;
}
