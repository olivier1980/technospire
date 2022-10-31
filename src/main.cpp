#include "Game/Game.h"

#include <sol/sol.hpp>

int main(int argc, char* argv[]) {


    srand (time(NULL));
    Game game; //created on stack (without NEW)
    game.Initialize();
    game.Run();
    game.Destroy();

    return 0;
}
