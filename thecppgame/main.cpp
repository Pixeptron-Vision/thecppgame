#include <iostream>
#include "game.h"
#include "worldmap.h"
#include "robot.h"
#include "utils.h"
int main(int argc, char** argv) {
  // Determine the game mode
  mode gameMode = askMode();
  Game G(gameMode);
  G.run();

  return 0;
}

/*

  World w(map);

  w.addRobot(1, 1);

  w.run();
*/
