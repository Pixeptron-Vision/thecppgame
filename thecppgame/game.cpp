#include "game.h"
#include "robot.h"
#include "utils.h"
#include "worldmap.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <cmath>
#include <mutex>

std::mutex gameMutex;
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

        int iterations = askNumberOfIterations();
        std::cout<<"Number of games suggested: "<<iterations<<"\n"<<std::endl;
        std::cout<<"\n -------------------------------------------------------------------\n"<<std::endl;
        World wmap(dimensions);

        int i=0;
        do
        {
            std::cout<<"***Setting up a New Game ***"<<std::endl;
            // Reset game parameters for next game
            // Reset game parameters for next game
            startGame = true; // Not used yet
            pauseGame = false; // Not used yet
            resumeGame = false;// Not used yet
            gameEnd = false; // Used to denote that robot agent has reached its destination
            quitGame = false; // Used to denote that game duration has ended and game must be shut down
            int obstacleCount = askNumberOfObstacles(wmap.getWorldDimensions());
            std::cout<<"Number of Obstacles suggested: "<<obstacleCount<<std::endl;
            wmap.setObstacleCount(obstacleCount);
            wmap.resetObstacleLocations();
            wmap.resetCollectibleLocations();
            wmap.resetHoleLocations();
            // remember the start time of the game
            gameStartTime =  std::chrono::high_resolution_clock::now();
            // Play the game
            gameEnd = playSingleAuto(wmap);
            std::cout<<"Time's Up!"<<std::endl;
            std::cout<<"Game Duration Ended"<<std::endl;
            if(!openThreads) std::cout<<"Attempt Successful!!"<<std::endl; else std::cout<<"Attempt Unsuccessful!! No path found!"<<std::endl;
            std::cout<<"\n -------------------------------------------------------------------\n"<<std::endl;
            // increment the game count
            i++;

        }while(i < iterations);

        break;

    }
  case single_manual:
    {

        int iterations = askNumberOfIterations();
        std::cout<<"Number of games suggested: "<<iterations<<"\n"<<std::endl;
        std::cout<<"\n -------------------------------------------------------------------\n"<<std::endl;
        World wmap(dimensions);

        int i=0;
        do
        {
            std::cout<<"***Setting up a New Game ***"<<std::endl;
            // Reset game parameters for next game
            startGame = true; // Not used yet
            pauseGame = false; // Not used yet
            resumeGame = false;// Not used yet
            gameEnd = false; // Used to denote that robot agent has reached its destination
            quitGame = false; // Not used yet
            int obstacleCount = askNumberOfObstacles(wmap.getWorldDimensions());
            std::cout<<"Number of Obstacles suggested: "<<obstacleCount<<std::endl;
            wmap.setObstacleCount(obstacleCount);
            wmap.resetObstacleLocations();
            wmap.resetCollectibleLocations();
            wmap.resetHoleLocations();
            // remember the start time of the game
            gameStartTime =  std::chrono::high_resolution_clock::now();
            // Play the game
            quitGame = playSingleManual(wmap);
            std::cout<<"Time's Up!"<<std::endl;
            std::cout<<"Game Duration Ended"<<std::endl;
            if(gameEnd)
            {
                std::cout<<"Attempt Successful!!"<<std::endl;
            }
            else
            {
                std::cout<<"Attempt Unsuccessful!! No path found!"<<std::endl;
            }

            if(openThreads)
            {
                std::cout<<"Waiting for user input (any digit) for the game to be closed!!"<<std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(7000));
                std::cout<<"Closing the Game!!"<<std::endl;

            }
            std::cout<<"\n -------------------------------------------------------------------\n"<<std::endl;
            // increment the game count
            i++;

        }while(i < iterations);
        break;
    }
  case multiple_auto:
    {
        int iterations = 1;
        //std::cout<<"Number of games suggested: "<<iterations<<"\n"<<std::endl;
        //std::cout<<"\n -------------------------------------------------------------------\n"<<std::endl;
        World wmap(dimensions);

        int i=0;
        do
        {
            std::cout<<"***Setting up a New Game ***"<<std::endl;
            // Reset game parameters for next game
            startGame = true; // Not used yet
            pauseGame = false; // Not used yet
            resumeGame = false;// Not used yet
            gameEnd = false; // Used to denote that robot agent has reached its destination
            quitGame = false; // Not used yet
            int obstacleCount = askNumberOfObstacles(wmap.getWorldDimensions());
            std::cout<<"Number of Obstacles suggested: "<<obstacleCount<<std::endl;
            wmap.setObstacleCount(obstacleCount);
            wmap.resetObstacleLocations();
            wmap.resetCollectibleLocations();
            wmap.resetHoleLocations();
            // remember the start time of the game
            gameStartTime =  std::chrono::high_resolution_clock::now();
            // Play the game
            quitGame = playMultipleAuto(wmap);
            std::cout<<"Time's Up!"<<std::endl;
            std::cout<<"Game Duration Ended"<<std::endl;
            if(gameEnd)
            {
                std::cout<<"Attempt Successful!!"<<std::endl;
            }
            else
            {
                std::cout<<"Attempt Unsuccessful!! No path found!"<<std::endl;
            }

            if(openThreads)
            {
                std::cout<<"Waiting for the game to be closed!!"<<std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(7000));
                std::cout<<"Closing the Game!!"<<std::endl;

            }
            std::cout<<"\n -------------------------------------------------------------------\n"<<std::endl;
            // increment the game count
            i++;

        }while(i < iterations);
        break;
    }
  }

  return gameEnd;
}


bool Game::spawnRobots(World& wmap, int numberOfRobots, direction head, float robotTimeUnit)
{
    /* SpawnRobots creates a robot agent object based on the mode of operation and spawns an
       individual thread for the robot to operate.
       Inputs: 1) Reference to a World object
               2) Number of robots i.e, no. of threads to be spawned
               3) Default direction for the robot's head
               4) Time delay factor for the robot operation (default 250)
       Output: Boolean Flag denoting success of the spawning operation
    */
    // Init Robot Parameters
    std::pair<int, int> startPosition = wmap.getStartLocation();
    std::pair<int, int> stopPosition = wmap.getStopLocation(true);
    std::cout<<"Start --> Stop : ("<<startPosition.first<<", "<<startPosition.second<<") --> ("<<stopPosition.first<<", "<<stopPosition.second<<")"<<std::endl;
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
                head = pickRandomDirection();
                Robot agent(startPosition, stopPosition, head, motionDirection, opMode, robotTimeUnit); // Initiialize robot with appropriate parameters
                agent.ID += i;
                switch (currentMode)
                {
                case single_auto:
                    {
                        // Evoke a thread for the robot object
                        std::thread t1(&Game::runRobotAuto, this, agent, wmap);
                        // std::this_thread :: sleep_for(std::chrono::milliseconds(1000));
                        thread_guard safe_thread(t1);
                        t1.detach();
                        openThreads = true;
                        //std::cout<<"Thread "<<i<<" Detached!"<<std::endl;
                        break;
                    }
                case single_manual:
                    {
                        // Evoke a thread for the robot object
                        std::thread t1(&Game::runRobotManual, this, agent, wmap);
                        // std::this_thread :: sleep_for(std::chrono::milliseconds(1000));
                        thread_guard safe_thread(t1);
                        t1.detach();
                        openThreads = true;
                        //std::cout<<"Thread "<<i<<" Detached!"<<std::endl;
                        break;
                    }
                case multiple_auto:
                    {
                        // Evoke a thread for the robot object
                        std::thread t1(&Game::runRobotsMultiple, this, agent, std::ref(wmap));
                        // std::this_thread :: sleep_for(std::chrono::milliseconds(1000));
                        thread_guard safe_thread(t1);
                        t1.detach();
                        openThreads = true;
                        //std::cout<<"Thread "<<i<<" Detached!"<<std::endl;
                        break;
                    }

                }
            }

    }
    catch (int &ex)
    {
        // Catch the exception and raise error ; set the failure status flag for return
        success = false;
        std::cerr<<"Exception in Spawning Threads, error: "<<ex<<std::endl;
    }

    return success;

}

/*
   ---------------------------------------------------------------------------------------------------------------------
   -----------------------------------------------------%% "Single-Auto" Mode Methods %% -------------------------------
   ---------------------------------------------------------------------------------------------------------------------
*/
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


void Game::runRobotAuto(Robot agent, World wmap)
{
    // Robot agent's operation thread.
    // Each robot agent follows the next sequence of instructions
    bool timeUp;

    do{
        // Execute BFS on the current map for determining the shortest path
        // Follow the shortest path and update the display continuously
        std::vector<std::pair<int, int>> path = agent.shortestPathBFS(wmap);
        // Print the Robot Path
        std::cout<<"Robot Path: "<<std::endl;
        for (auto it = path.rbegin(); it!=path.rend(); ++it)
        {
            std::cout<<(*it).first<<","<<(*it).second<<" --> ";
            wmap.setLocationTrail(*it, '_');
        }
        std::cout<<"|\n";
        wmap.setLocationTrail(wmap.getStartLocation(), 'a');
        wmap.setLocationTrail(wmap.getStopLocation(), '@');
        timeUp = timeOut();
        gameEnd = true;
    }while(!(timeUp || gameEnd));
    std::cout<<"Status: Time Up - "<<timeUp<<"; Game End Signal - "<<gameEnd<<std::endl;
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
    std::cout<<"Waiting for Game time to finish!"<<std::endl;
    // Reset the openThreads flag as the thread is about to end operation
    openThreads = false;
}

/*
   ---------------------------------------------------------------------------------------------------------------------
   -----------------------------------------------------%% "Single-Manual" Mode Methods %% -----------------------------
   ---------------------------------------------------------------------------------------------------------------------
*/
bool Game::playSingleManual(World& wmap)
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


void Game::runRobotManual(Robot agent, World wmap)
{
    // Robot agent's operation thread.
    // Each robot agent follows the next sequence of instructions
    bool timeUp;
    char content;
    wmap.setLocationTrail(wmap.getStartLocation(), 'a');
    wmap.setLocationTrail(wmap.getStopLocation(), '@');
    std::vector<std::pair<int, int>> path;
    std::pair<int, int> nextStep;
    do{


        // Take input from the user
        std::pair<int, int> location = agent.getCurrentLocation();
        //std::cout<<"Current Location: ("<<location.first<<", "<<location.second<<")"<<std::endl;
        nextStep = askUserForNextStep(wmap, location);
        if(nextStep.first == -1 && nextStep.second == -1)
        {
            std::cout<<"Game Aborted!!\n";
            gameEnd = true;
            break;
        }
        content = wmap.getLocationContent(nextStep);
        if (content== '@')
        {
            gameEnd = true;
            break;
        }
        else if(content == '+')
        {
            agent.life += 1;
        }
        else if (content == 'O')
        {
            agent.life -= 1;
        }
        wmap.setLocationContent(nextStep, '~');
        wmap.setLocationTrail(nextStep, '~');
        agent.setCurrentLocation(nextStep);
        wmap.show();
        timeUp = timeOut();

    }while(!((timeUp || gameEnd || quitGame)|| agent.life == 0));
    std::cout<<"Status: Time Up - "<<timeUp<<"; Game End Signal - "<<gameEnd<<std::endl;
    std::cout<<"Robot Life Left: "<<agent.life<<std::endl;
    std::cout<<"Map with Robot Path: "<<std::endl;
    wmap.show(4);

    // Reset the openThreads flag as the thread is about to end operation
    openThreads = false;
    std::cout<<"Robot Operation Ended!"<<std::endl;
    std::cout<<"Waiting for Game time to finish!"<<std::endl;

}

/*
   ---------------------------------------------------------------------------------------------------------------------
   -----------------------------------------------------%% "Multiple-Auto" Mode Methods %% -----------------------------
   ---------------------------------------------------------------------------------------------------------------------
*/
bool Game::playMultipleAuto(World &wmap)
{
    //direction head = askHeadDirection();
    // remember the start time of the game
    gameStartTime =  std::chrono::high_resolution_clock::now();

    // Spawn the robot threads
    // initialize the robot params in this mode
    activeRobots = askNumberOfRobots();
    bool robotinit = spawnRobots(wmap, activeRobots);
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
            // Give delay for next step
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            wmap.show();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

        }while(-gameTimeDuration.count() < gameTimeLimit.count());
    }

    return true;
}


void Game::runRobotsMultiple(Robot agent, World& wmap)
{
    // Robot agent's operation thread.
    // Each robot agent follows the next sequence of instructions

    // Init parameters
    bool timeUp, validLocation;
    char content= '*';
    timeUp =false;
    validLocation = false;
    std::vector<std::pair<int, int>> path;
    std::pair<int, int> nextStep;
    // perform action
    do{
        // Determine the current location of the agent
        std::pair<int, int> location = agent.getCurrentLocation();
        //std::cout<<"Current Location: ("<<location.first<<", "<<location.second<<")"<<std::endl;
        // Add current location to the agent's path
        path.push_back(location);
        // Determine the next step of the agent
        nextStep = agent.determineNextStep();
        gameMutex.lock(); // lock the resource
        // Determine next step content
        content = wmap.getLocationContent(nextStep);
        switch(content)
        {
            case '@':
                {
                    gameEnd = true;
                    break;
                }
            case '+':
                {
                    agent.life += 1;
                    break;
                }
            case 'O':
                {
                    agent.life -= 1;
                }
        }
        // Validate next step
        validLocation = wmap.moveFromTo(location, nextStep, agent.ID); // locationContent unused at the moment
        gameMutex.unlock(); // unlock the resource
        //std::cout<<"Content - Next Step: "<<content<<"; Validity: "<<validLocation<<std::endl;
        if (!validLocation)
        {
            // Change the direction of the robot's head in case the next location identified is invalid
            agent.changeHeadDirection();
        }
        else
        {
            // For a valid next location:
            // update the current location and perform appropriate operations based on the location's content
            agent.setCurrentLocation(nextStep);
        }
        // Give delay for next step
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        //wmap.show(); // Not necessary since it is displayed in the parent thread; may be used for debugging
        timeUp = timeOut();

    }while(!((timeUp || gameEnd || quitGame)|| agent.life == 0));
    std::cout<<"Status: Time Up - "<<timeUp<<"; Game End Signal - "<<gameEnd<<std::endl;
    std::cout<<"Robot "<<agent.ID<<"'s Life Left: "<<agent.life<<std::endl;
    // Reset the openThreads flag as the thread is about to end operation
    openThreads = false;
    std::cout<<"Robot Operation Ended!"<<std::endl;
    std::cout<<"Waiting for Game time to finish!"<<std::endl;
}

/*
   ---------------------------------------------------------------------------------------------------------------------
   ---------------------------------------------------- %% Timing Methods %% -------------------------------------------
   ---------------------------------------------------------------------------------------------------------------------
*/

std::chrono::milliseconds Game:: determineGameTime(std::pair<int, int> dimensions, float robotTimeUnit, float agilityFactor)
{
    /*
        This method is used to determine the amount of time main thread has to run before it ends
        the game and calls "time-out". The time formula and calculations done for each game modes are based on
        trial and error for a smooth experience.

        Inputs: 1) Dimensions of the world map
                2) Time taken by robot for each step (assume it as time delay given in the robot thread for each step)
                3) Agility factor to make the game interesting on various game levels (Not varied now, default value in use)

        Output: Game duration in milliseconds
    */

    float shortestPathFactor, totalTime;
    switch(currentMode)
    {
    case single_auto:
        {
            shortestPathFactor = (std::sqrt(dimensions.first * dimensions.first  + dimensions.second * dimensions.second ) );
            totalTime = shortestPathFactor * agilityFactor * robotTimeUnit;
            break;
        }
    case single_manual:
        {
            shortestPathFactor = (std::sqrt(dimensions.first * dimensions.first  + dimensions.second * dimensions.second ) );
            totalTime = shortestPathFactor * agilityFactor * robotTimeUnit;
            totalTime *= 10;
            break;
        }
    case multiple_auto:
        {
            shortestPathFactor = (std::sqrt(dimensions.first * dimensions.first  + dimensions.second * dimensions.second ) );
            totalTime = shortestPathFactor * agilityFactor * robotTimeUnit;
            totalTime *= 10;
            break;
            break;
        }
    }

    //std::cout<<totalTime<<", "<<shortestPathFactor<<", "<<agilityFactor<<", "<<robotTimeUnit<<std::endl;
    // Convert float into chrono milliseconds
    std::chrono::milliseconds totalTimeMS{static_cast<long int>(totalTime)}; // Create a chrono ms object
    gameTimeLimit = totalTimeMS; // initialize the gameTimeLimit from the earlier object
    //std::cout<<"Total Time: "<<totalTime<<std::endl;

    return gameTimeLimit;
}


bool Game:: timeOut()
{
    /*  Method to identify whether or not the game time is completed
        Inputs: None
        Output: Timeout flag
    */
    // Determine the time at the moment
    auto timeNow = std::chrono::high_resolution_clock::now();
    // Determine the time duration from start to now
    gameTimeDuration = std::chrono::duration_cast<std::chrono::milliseconds>( gameStartTime - timeNow); // Negative Number
    //std::cout<<"Robot: "<<gameTimeDuration.count()<<"  /  "<<gameTimeLimit.count()<<std::endl;
    // Check if the time duration is complete
    if (-gameTimeDuration.count() < gameTimeLimit.count())
    {
        return false;
    }
    else
    {
        std::cout<<"Time Out!! Game Up!\n";
        return true;
    }
}

