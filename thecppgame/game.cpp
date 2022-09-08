#include "game.h"
#include "robot.h"
#include "utils.h"
#include "worldmap.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <cmath>
Game:: Game(mode gameMode): currentMode{gameMode}, activeRobots{0}, level{0}{}
Game:: Game():currentMode{single_auto},activeRobots{0},level{0}{}
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

  // remember the start time of the game
  gameStartTime =  std::chrono::high_resolution_clock::now();
  switch(currentMode)
  {
  case single_auto:
    {
        startGame = true;
        quitGame = false;
        pauseGame = false;
        resumeGame = false;
        gameEnd = playSingleAuto(wmap);

        break;
    }
  case single_manual:
    {
        startGame = true;
        quitGame = false;
        pauseGame = false;
        resumeGame = false;
        break;
    }
  case multiple_auto:
    {
        startGame = true;
        quitGame = false;
        pauseGame = false;
        resumeGame = false;
        break;
    }
  }

  return gameEnd;
}


void Game::runRobot(Robot agent)
{
    // Robot agent's operation thread.
    // Each robot agent follows the next sequence of instructions
    bool jobDone = false;
    std::cout<<"In runRobot "<<std::endl;
    do{
        // Execute BFS on the current map for determining the shortest path
        // Follow the shortest path and update the display continuously

        auto timeNow = std::chrono::high_resolution_clock::now();
        gameTimeDuration = std::chrono::duration_cast<std::chrono::milliseconds>( gameStartTime - timeNow);
        //std::cout<<"Robot: "<<gameTimeDuration.count()<<"  /  "<<gameTimeLimit.count()<<std::endl;

    }while(-gameTimeDuration.count() < gameTimeLimit.count() && jobDone == false);


}


bool Game::spawnRobots(int numberOfRobots, std::pair<int, int> startPosition, std::pair<int, int> stopPosition,
                     direction head, bool motionDirection, mode opMode, float robotTimeUnit)
{
    bool success {true};
    // Spawn the robot agents
    try
    {
        for(int i=0; i< numberOfRobots; ++i)
            {
                // Create a robot object
                Robot agent(startPosition, stopPosition, head, motionDirection, opMode, robotTimeUnit); // Initiialize robot with appropriate parameters
                // Evoke a thread for the robot object
                std::thread t1(&Game::runRobot, this, agent);
                // std::this_thread :: sleep_for(std::chrono::milliseconds(1000));
                thread_guard safe_thread(t1);
                t1.detach();
                std::cout<<"Thread "<<i<<" Detatched!"<<std::endl;
            }
    }
    catch (int &ex)
    {
        success = false;
        std::cerr<<"Exception in Spawning Threads, error: "<<ex<<std::endl;
    }

    return success;

}


std::chrono::milliseconds Game:: determineGameTime(std::pair<int, int> dimensions, float robotTimeUnit, float agilityFactor)
{
    //float shortestPathFactor = (std::sqrt(dimensions.first * dimensions.first  + dimensions.second * dimensions.second )/ (dimensions.first * dimensions.second));
    float shortestPathFactor = (std::sqrt(dimensions.first * dimensions.first  + dimensions.second * dimensions.second ) );
    float totalTime = shortestPathFactor * agilityFactor * robotTimeUnit;
    //std::cout<<totalTime<<", "<<shortestPathFactor<<", "<<agilityFactor<<", "<<robotTimeUnit<<std::endl;
    // Convert float into chrono milliseconds
    std::chrono::milliseconds totalTimeMS{static_cast<long int>(totalTime)}; // Create a chrono ms object
    gameTimeLimit = totalTimeMS; // initialize the gameTimeLimit from the earlier object

    return gameTimeLimit;
}


bool Game:: playSingleAuto(World& wmap)
{
    // Spawn a robot thread
    // initialize the robot params in this mode
    std::cout<<"In Play "<<std::endl;
    // Display the map on to the screen
    wmap.show();
    std::pair<int, int> startPosition = wmap.getStartLocation();
    std::pair<int, int> stopPosition = wmap.getStopLocation();
    // Set the game time limits
    gameTimeLimit = determineGameTime(wmap.getWorldDimensions());
    bool robotinit = spawnRobots(1, startPosition, stopPosition);

    if (robotinit)
    {
        // threads spawned successfully.
        // Set the game status booleans

        // Now keep time and end the game after the designated time
        do{
            auto timeNow = std::chrono::high_resolution_clock::now();
            gameTimeDuration = std::chrono::duration_cast<std::chrono::milliseconds>( gameStartTime - timeNow);
            //std::cout<<"Main: "<<gameTimeDuration.count()<<"  /  "<<gameTimeLimit.count()<<std::endl;

        }while(-gameTimeDuration.count() < gameTimeLimit.count());
    }
    std::cout<<"Game Duration Ended"<<std::endl;

    return true;
}


bool playSingleManual()
{
    return true;
}


bool playMultipleAuto()
{
    return true;
}

