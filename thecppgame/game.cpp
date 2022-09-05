#include "game.h"
#include "utils.h"
#include "worldmap.h"
#include <iostream>
Game:: Game(mode gameMode): currentMode{gameMode}, activeRobots{0}{}
Game:: Game(){}
Game:: ~Game(){}
bool Game:: run()
{
  // *****  Initialize the program by requesting user the map details ****

  // Take the size estimate for the map
  std::pair<int, int> dimensions = askMapDimensions();
  std::cout<<dimensions.first<< " , "<<dimensions.second<<std::endl;

  int obstacleCount = askNumberOfObstacles(dimensions);
  std::cout<<"Number of Obstacles suggested: "<<obstacleCount<<std::endl;

  int iterations = askNumberOfIterations();
  std::cout<<"Number of Iterations suggested: "<<iterations<<std::endl;
  // Initialize the Map
  World wmap(dimensions, obstacleCount, iterations);
  // Display the map on to the screen
  wmap.show();



  return true;
}
