#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <random>
#include "worldmap.h"
#include "robot.h"
// Function Definitions
World::World(std::pair<int, int> dimensions,int obstacleCount, int robotCount): Game{},
    worldMapDimensions{dimensions}, numberOfObstacles{obstacleCount}, numberOfRobots{robotCount}
{
    std::cout<<"Initialization"<<std::endl;
    // Map creation variables
    //std::vector<std::pair<int, std::pair<int, int>>> robotLocationMapping;

    // Robot Movement variables
    //std::vector<bool> robotMapAccess;
    // Initialize obstacleLocations and worldMap
    createMap();




    //
}
// Constructor Delegations
World::World(): World{{5,5}, 2, 1}{}
World::World(std::pair<int, int> dimensions): World{dimensions, 2, 1}{}
World::World(std::pair<int, int> dimensions,int obstacleCount): World{dimensions, obstacleCount, 1}{}
World::~World(){};

// Private Functions
void World::createMap()
{
    /* Map Creation::
    Example of a 5 x 5 Map is as follows:

    *******
    *---X-*
    *-A---*
    *-----*
    *-X---*
    *---X-*
    *******

    Where,
    1) * Denotes boundary
    2) - Denotes empty space
    3) X denotes obstacle
    4) Capital alphabets (A, B, C,...) denote the Robots

    */

    // Initialize the map variables
    int length = worldMapDimensions.first;
    int breadth = worldMapDimensions.second;

    // Map Creation Step - 1: Initiate a 2D Vector
    // Create map empty space template
    std::vector<char> buildingBlock(breadth + 2, '-');

    // Add boundaries to the space
    auto iter = buildingBlock.begin();
    *iter = '*';
    iter = buildingBlock.end();
    --iter;
    *iter = '*';

    // Create Boundary Template
    std::vector<char> boundary(breadth + 2, '*');

    // Add the upper boundary to the map
    worldMap.push_back(boundary);

    // Add the spaces to the map
    for (int i=0; i < (length); ++i)
    {
        worldMap.push_back(buildingBlock);
    }

    // Add lower boundary to the map
    worldMap.push_back(boundary);

    // Generate obstacle location list
    generateObstacleLocations();

    // Add obstacles to the map
    for (auto location: obstacleLocations)
    {
        worldMap[location.first][location.second] = 'X';
    }

    // Add Robots to the map
    for (auto mapping : robotLocationMapping)
    {
        int robotID = mapping.first;
        std:: pair<int, int> robotLocation = mapping.second;
        worldMap[robotLocation.first][robotLocation.second] = robotID;
        std::cout<<"Yes"<<std::endl;
    }
}


std::vector<std::pair<int, int>> World:: generateCoordinates(int distributionRange, int numberOfCoordinates)
{
    /* Random coordinate generation code taken from stackoverflow
    Reference: https://stackoverflow.com/questions/26086128/generate-different-random-coordinates
    */
    std::vector<std::pair<int, int>> coordinates;
    // Generate random locations in the map for the obstacles
    std::mt19937 eng; // object which produces random bits
    std::random_device r;
    std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
    eng.seed(seed); // seed the bit generator, replaces srand()
    std::uniform_int_distribution<> dist(1, distributionRange); // encapsulates the correct method of turning
                                                                // random bits into random numbers in the range [0, N)
    for(int i = 0; i < numberOfCoordinates; i++)
    {
        int x = dist(eng); // the distribution internally used the engine like rand()
        int y = dist(eng);
        std::pair<int, int> v = {x, y}; // create a coordinate variable for comparison with the ones in the list
        // create function for std::pair comparison
        auto is_equal = [&v](const std::pair<int, int> &i){return (i.first == v.first) && (i.second == v.second);};
        // find if the coordinate has already been generated
        auto check = std::find_if(coordinates.begin(), coordinates.end(), is_equal);
        if (check == std::end(coordinates))
        {
            coordinates.push_back({x, y});
        }
        else{
            // if the coordinate has been generated earlier, reduce the iteration count
            --i;
        }

    }

    return  coordinates;
}


void World::generateObstacleLocations()
{
    // Determine the lowest dimension of the map
    int N = worldMapDimensions.first < worldMapDimensions.second ? worldMapDimensions.first : worldMapDimensions.second;
    obstacleLocations = generateCoordinates(N, numberOfObstacles);
    for(auto loc : obstacleLocations)std::cout<<loc.first<<","<<loc.second<<" ";
    std::cout<<"\n";
}

/*
Robot World::addRobot(std::pair<int, int> startPosition, direction head, bool motion, bool mode, bool status)
{
    // Create a new thread for the Robot

    // Initialize a Robot
    Robot robotInstance(startPosition, head, motion, mode, status);


    // Run the robot


}


*/

void World::show()
{
    int length = worldMapDimensions.first;
    int breadth = worldMapDimensions.second;
    // Print
    for (auto &row : worldMap)
    {
        for(auto &col : row)
        {
            std::cout<<" "<<col<<" ";
        }
        std::cout<<"\n";
    }

}

/*
bool World::addRobot(int row, int col) {
  map_[row][col] = 'A';
  return true;
}

void World::run() {
  while (true) {
    display();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}


*/

