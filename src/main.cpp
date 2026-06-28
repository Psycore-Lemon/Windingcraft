#include "core/Engine.h"

int main()
{
    Engine engine;

    if (!engine.Init("config.json"))
        return -1;

    engine.Run();

    return 0;
}
