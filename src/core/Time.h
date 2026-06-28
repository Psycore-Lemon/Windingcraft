#pragma once

class Time
{
public:
    void Update();
    double DeltaTime() const;

private:
    double lastTime = 0.0;
    double deltaTime = 0.0;
};
