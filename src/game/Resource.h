#pragma once

struct ResourcePool
{
    float current;
    float max;

    float Ratio() const { return (max > 0.0f) ? current / max : 0.0f; }
    bool Empty() const { return current <= 0.0f; }
    bool Full() const { return current >= max; }

    void Spend(float amount) { current -= amount; if (current < 0.0f) current = 0.0f; }
    void Restore(float amount) { current += amount; if (current > max) current = max; }
};

struct Vitals
{
    ResourcePool health;
    ResourcePool mana;
    ResourcePool stamina;
};
