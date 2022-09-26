#include <iostream>
#include "game.h"
#include "worldmap.h"
#include "robot.h"
#include "utils.h"
int main(int argc, char** argv) {
  // Determine the game mode
  mode gameMode = askMode();
  // Initiate a game object
  Game G(gameMode);
  // Run the game
  G.run();

  return 0;
}
