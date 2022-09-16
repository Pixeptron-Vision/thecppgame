#include "game.h"
#include "robot.h"
#include "utils.h"
#include "worldmap.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <cmath>
Game:: Game(mode gameMode): currentMode{gameMode}, activeRobots{0}, level{0}, openThreads{false}{}
Game:: Game():currentMode{single_auto}, activeRobots{0}, level{0}, openThreads{false}{}
Game:: ~Game(){}


bool Game:: run()
{
  // *****  Initialize the program by requesting user the map details ****
  // Take the size estimate for the map
  std::pair<int, int> dimensions = askMapDimensions();
  std::cout<<dimensions.first<< " , "<<dimensions.second<<std::endl;

  switch(currentMode)
  {
  case single_auto:
    {
        startGame = true;
        quitGame = false;
        pauseGame = false;
        resumeGame = false;

        int iterations = askNumberOfIterations();
        std::cout<<"Number of games suggested: "<<iterations<<"\n"<<std::endl;
        std::cout<<"\n -------------------------------------------------------------------\n"<<std::endl;
        World wmap(dimensions);

        int i=0;
        do
        {
            std::cout<<"***Setting up a New Game ***"<<std::endl;
            // Reset game parameters for next game
            int obstacleCount = askNumberOfObstacles(wmap.getWorldDimensions());
            std::cout<<"Number of Obstacles suggested: "<<obstacleCount<<std::endl;
            wmap.setObstacleCount(obstacleCount);
            wmap.resetObstacleLocations();

            // remember the start time of the game
            gameStartTime =  std::chrono::high_resolution_clock::now();
            // Play the game
            gameEnd = playSingleAuto(wmap);
            std::cout<<"Time's Up!"<<std::endl;
            std::cout<<"Game Duration Ended"<<std::endl;
            if(!openThreads) std::cout<<"Attempt Successful!!"<<std::endl; else std::cout<<"Attempt Unsuccessful!!"<<std::endl;
            std::cout<<"\n -------------------------------------------------------------------\n"<<std::endl;
            // increment the game count
            i++;

        }while(i < iterations);

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
            wmap.setLocationTrail(*it, '_');
        }
        std::cout<<"|\n";
        timeUp = timeOut();

    }while(!(timeUp || gameEnd));
    std::cout<<"Robot Life Left: "<<agent.life<<std::endl;
    std::cout<<"Map with Robot Path: "<<std::endl;
    wmap.show(4);
    /*
    wmap.show(1);
    std::cout<<"Previous"<<std::endl;
    wmap.show(2);
    std::cout<<"Next"<<std::endl;
    wmap.show(3);
    */
    std::cout<<"Robot Operation Ended!"<<std::endl;
    openThreads = false;
}


bool Game::spawnRobots(World wmap, int numberOfRobots, direction head, float robotTimeUnit)
{
    // Init Robot Parameters
    std::pair<int, int> startPosition = wmap.getStartLocation();
    std::pair<int, int> stopPosition = wmap.getStopLocation(true);
    bool motionDirection= true;
    mode opMode = single_auto;
    // Display the map on to the screen
    wmap.show();
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
                openThreads = true;
                //std::cout<<"Thread "<<i<<" Detached!"<<std::endl;
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
    //direction head = askHeadDirection();
    // remember the start time of the game
    gameStartTime =  std::chrono::high_resolution_clock::now();
    // Spawn a robot thread
    // initialize the robot params in this mode
    //std::cout<<"In Play "<<std::endl;
    bool robotinit = spawnRobots(wmap, 1);
    // Set the game time limits
    gameTimeLimit = determineGameTime(wmap.getWorldDimensions());

    if (robotinit)
    {
        // threads spawned successfully.
        // Now keep time and end the game after the designated time
        do{
            auto timeNow = std::chrono::high_resolution_clock::now();
            gameTimeDuration = std::chrono::duration_cast<std::chrono::milliseconds>( gameStartTime - timeNow);
            //std::cout<<"Main: "<<gameTimeDuration.count()<<"  /  "<<gameTimeLimit.count()<<std::endl;

        }while(-gameTimeDuration.count() < gameTimeLimit.count());
    }

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

