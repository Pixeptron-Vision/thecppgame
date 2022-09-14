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


bool Game:: timeOut()
{
    auto timeNow = std::chrono::high_resolution_clock::now();
    gameTimeDuration = std::chrono::duration_cast<std::chrono::milliseconds>( gameStartTime - timeNow); // Negative Number
    //std::cout<<"Robot: "<<gameTimeDuration.count()<<"  /  "<<gameTimeLimit.count()<<std::endl;
    if (-gameTimeDuration.count() < gameTimeLimit.count())
    {
        return false;
    }
    else
    {
        return true;
    }
}


void Game::runRobot(Robot agent, World wmap)
{
    // Robot agent's operation thread.
    // Each robot agent follows the next sequence of instructions
    bool timeUp;

    do{
        // Execute BFS on the current map for determining the shortest path
        // Follow the shortest path and update the display continuously
        std::vector<std::pair<int, int>> path = agent.shortestPathBFS(wmap);
        gameEnd = true;
        std::cout<<"Robot Path: "<<std::endl;
        for (auto it = path.rbegin(); it!=path.rend(); ++it)
        {
            std::cout<<(*it).first<<","<<(*it).second<<" --> ";
            wmap.setLocationContent(*it, ',');
        }
        std::cout<<"\n";
        timeUp = timeOut();

    }while(!(timeUp || gameEnd));
    std::cout<<"Map with Robot Path: "<<std::endl;
    wmap.show();
    /*
    wmap.show(1);
    std::cout<<"Previous"<<std::endl;
    wmap.show(2);
    std::cout<<"Next"<<std::endl;
    wmap.show(3);
    */
    std::cout<<"Closing the thread!"<<std::endl;

}


bool Game::spawnRobots(World wmap, int numberOfRobots, float robotTimeUnit)
{
    // Init Robot Parameters
    std::pair<int, int> startPosition = wmap.getStartLocation();
    std::pair<int, int> stopPosition = wmap.getStopLocation();
    direction head = North;
    bool motionDirection= true;
    mode opMode = single_auto;

    // Init operation status variable
    bool success {true};

    // Spawn the robot agents
    try
    {
        for(int i=0; i< numberOfRobots; ++i)
            {
                // Create a robot object
                Robot agent(startPosition, stopPosition, head, motionDirection, opMode, robotTimeUnit); // Initiialize robot with appropriate parameters
                // Evoke a thread for the robot object
                std::thread t1(&Game::runRobot, this, agent, wmap);
                // std::this_thread :: sleep_for(std::chrono::milliseconds(1000));
                thread_guard safe_thread(t1);
                t1.detach();
                std::cout<<"Thread "<<i<<" Detached!"<<std::endl;
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

    // Set the game time limits
    gameTimeLimit = determineGameTime(wmap.getWorldDimensions());
    bool robotinit = spawnRobots(wmap);

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

