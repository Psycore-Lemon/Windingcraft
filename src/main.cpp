#include "core/Game.h"

int main()
{
    Game game;

    if (!game.Init("config.json"))
        return -1;

    game.Run();

    return 0;
}
