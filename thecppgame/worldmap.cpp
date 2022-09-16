#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <random>
#include <cstdlib>
#include "worldmap.h"
#include "robot.h"
// Function Definitions
World::World(std::pair<int, int> dimensions,int obstacleCount, int robotCount): Game{},
    worldMapDimensions{dimensions}, numberOfObstacles{obstacleCount}, numberOfRobots{robotCount}
{
    std::cout<<"Initializing the World!!"<<std::endl;
    // Map creation variables initialization
    // Determine the lowest dimension of the map to compute distributionRange
    distributionRange = worldMapDimensions.first < worldMapDimensions.second ? worldMapDimensions.first : worldMapDimensions.second;

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
    std::vector<Arena> buildingBlock(breadth + 2);
    for(auto it = buildingBlock.begin(); it!=buildingBlock.end(); ++it)
    {
        (*it).content = '-';
        (*it).flag = false;
        (*it).previous = {0,0};
        (*it).next = {0,0};
        (*it).trail = '*';
    }
    // Add boundaries to the space
    auto iter = buildingBlock.begin();
    (*iter).content = '*';
    iter = buildingBlock.end();
    --iter;
    (*iter).content = '*';

    // Create Boundary Template
    std::vector<Arena> boundary(breadth + 2);
    for(auto it = boundary.begin(); it!=boundary.end(); ++it)
    {
        (*it).content = '*';
        (*it).flag = false;
        (*it).previous = {0,0};
        (*it).next = {0,0};
        (*it).trail = '*';
    }

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
    addToMap(obstacleLocations,'X');

    // Add collectibles to the map
    numberOfCollectibles = int(numberOfObstacles * 0.15);
    collectibleLocations = generateCoordinates(distributionRange, numberOfCollectibles);
    addToMap(collectibleLocations, '+');

    // Add holes to the map
    numberOfHoles = int ((level + 1) * 0.1 * numberOfObstacles);
    holesLocations = generateCoordinates(distributionRange, numberOfHoles);
    addToMap(holesLocations, 'O');


    if (currentMode == single_auto || currentMode == single_manual)
    {
        // Mark robot destination on the map
        destinationLocation.push_back({worldMapDimensions.first, worldMapDimensions.second});
        addToMap(destinationLocation, '@');

        // Add Robot on the map
        startingLocations.push_back({1,1});
        addToMap(startingLocations, 'a');

    }
    else{
        // Add Robots to the map
        for (auto mapping : robotLocationMapping)
        {
            int robotID = mapping.first;
            std:: pair<int, int> robotLocation = mapping.second;
            (worldMap[robotLocation.first][robotLocation.second]).content = robotID;
        }
    }

}


void World:: addToMap(std::vector<std::pair<int, int>> coordinateList, char symbol)
{
    for (auto location: coordinateList)
    {
        (worldMap[location.first][location.second]).content = symbol;
    }

}


void World::setLocationNext(std::pair<int, int> location,std::pair<int, int> information)
{
    (worldMap[location.first][location.second]).next.first = information.first;
    (worldMap[location.first][location.second]).next.second = information.second;
}


void World::setLocationPrevious(std::pair<int, int> location,std::pair<int, int> information)
{
    (worldMap[location.first][location.second]).previous.first = information.first;
    (worldMap[location.first][location.second]).previous.second = information.second;
}


void World::setLocationContent(std::pair<int, int> location,char information)
{
    (worldMap[location.first][location.second]).content = information;
}


void World::setLocationTrail(std::pair<int, int> location,char information)
{
    (worldMap[location.first][location.second]).trail = information;
}


void World::setLocationFlag(std::pair<int, int> location,bool information)
{
    (worldMap[location.first][location.second]).flag = information;
}


std::pair<int, int> World :: getStartLocation(bool generateNewLocation)
{
    if (generateNewLocation)
    {
        char agentID = (worldMap[startingLocations[0].first][startingLocations[0].second]).content;
        std::cout<<"ID :"<<agentID<<std::endl;
        addToMap(startingLocations, '-');
        // Generate 20 feasible coordinates randomly from the map
        std::vector<std::pair<int, int>> newLocation = generateCoordinates(distributionRange, 20);
        // Amongst the coordinate list, pick a single coordinate randomly
        std::srand((unsigned) std::time(NULL)); // provide a seed value
        int offset = 0;
        int range = 19; // same as (the number of elements - 1) in newLocation vector
        int random_index = offset + (std::rand() % range); // generate the index for random selection from list
        startingLocations[0].first = newLocation[random_index].first;
        startingLocations[0].second = newLocation[random_index].second;
        addToMap(startingLocations, agentID);
    }

    return startingLocations[0];
}


std::pair<int, int> World:: getStopLocation(bool generateNewLocation)
{
    if (generateNewLocation)
    {
        addToMap(destinationLocation, '-');
        // Generate 20 feasible coordinates randomly from the map
        std::vector<std::pair<int, int>> newLocation = generateCoordinates(distributionRange, 20);
        // Amongst the coordinate list, pick a single coordinate randomly
        std::srand((unsigned) std::time(NULL)); // provide a seed value
        int offset = 0;
        int range = 19; // same as (the number of elements - 1) in newLocation vector
        int random_index = offset + (std::rand() % range); // generate the index for random selection from list
        destinationLocation[0].first = newLocation[random_index].first;
        destinationLocation[0].second = newLocation[random_index].second;
        addToMap(destinationLocation, '@');
    }
    return destinationLocation[0];
}


bool World::getLocationFlag(std::pair<int, int> location)
{
    return (worldMap[location.first][location.second]).flag;
}


char World::getLocationContent(std::pair<int, int> location)
{
     return (worldMap[location.first][location.second]).content;
}


char World::getLocationTrail(std::pair<int, int> location)
{
     return (worldMap[location.first][location.second]).trail;
}


std::pair<int,int> World::getLocationPrevious(std::pair<int, int> location)
{
    return (worldMap[location.first][location.second]).previous;
}


std::pair<int,int> World::getLocationNext(std::pair<int, int> location)
{
    return (worldMap[location.first][location.second]).next;
}


std::vector<std::pair<int, int>> World:: getNeighborList(std::pair<int, int> location,direction headDirection, bool clockwiseHeadRotation)
{
    std::vector<std::pair<int, int>> neighbors;
    //std::cout<<location.first<<", "<<location.second<<"\n"; //optional to check the coordinates
    if (clockwiseHeadRotation)
    {
        // Add the east neighbor
        neighbors.push_back({location.first, location.second + 1});
        // Add the south east neighbor
        neighbors.push_back({location.first + 1, location.second + 1});
        // Add the south neighbor
        neighbors.push_back({location.first + 1, location.second});
        // Add the south west neighbor
        neighbors.push_back({location.first + 1, location.second - 1});
        // Add the west neighbor
        neighbors.push_back({location.first , location.second - 1});
        // Add the north west neighbor
        neighbors.push_back({location.first - 1, location.second - 1});
        // Add the north neighbor
        neighbors.push_back({location.first - 1, location.second });
        // Add the north east neighbor
        neighbors.push_back({location.first - 1, location.second + 1});

    }
    else
    {
        // Add the east neighbor
        neighbors.push_back({location.first, location.second + 1});
        // Add the north east neighbor
        neighbors.push_back({location.first - 1, location.second + 1});
        // Add the north neighbor
        neighbors.push_back({location.first - 1, location.second });
        // Add the north west neighbor
        neighbors.push_back({location.first - 1, location.second - 1});
        // Add the west neighbor
        neighbors.push_back({location.first , location.second - 1});
        // Add the south west neighbor
        neighbors.push_back({location.first + 1, location.second - 1});
        // Add the south neighbor
        neighbors.push_back({location.first + 1, location.second});
        // Add the south east neighbor
        neighbors.push_back({location.first + 1, location.second + 1});

    }
    // Rotate the neighbors list to make sure head points the first neighbor
    switch(headDirection)
    {
    case East:
        {
            return neighbors;
        }
    case SouthEast:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+1,neighbors.end());
            break;
        }
    case South:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+2,neighbors.end());
            break;
        }
    case SouthWest:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+3,neighbors.end());
            break;
        }
    case West:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+4,neighbors.end());
            break;
        }
    case NorthWest:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+5,neighbors.end());
            break;
        }
    case North:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+6,neighbors.end());
            break;
        }
    case NorthEast:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+7,neighbors.end());
            break;
        }
    default:
        {
            // default East
            return neighbors;
        }
    }
    return neighbors;
}


std::vector<std::pair<int, int>> World:: getTraversibleNeighborList(std::pair<int, int> location,direction head, bool clockwiseHeadRotation)
{
    std::vector<std::pair<int, int>> neighbors = getNeighborList(location, head, clockwiseHeadRotation);
    std::vector<std::pair<int, int>> filteredNeighbors;

    for(auto it = neighbors.begin(); it!=neighbors.end(); ++it)
    {
        if (!(worldMap[(*it).first][(*it).second].content== '*' ||  worldMap[(*it).first][(*it).second].content== 'X'))
        {
            filteredNeighbors.push_back({(*it).first, (*it).second});
        }
    }
    /*
    std::cout<<"Filtered Neighbors\n";
    for(auto it=filteredNeighbors.begin(); it!=filteredNeighbors.end(); ++it)
    {
        std::cout<<(*it).first<<", "<<(*it).second<<std::endl; // Optional to check the coordinates read
    }
    */
    return filteredNeighbors;
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

void World::setObstacleCount(int obstacleCount)
{
    numberOfObstacles = obstacleCount;
}


void World::setCollectibleCount(int collectibleCount)
{
    numberOfCollectibles = collectibleCount;
}


void World::setHoles(int holesCount)
{
    numberOfHoles = holesCount;
}


void World::resetCollectibleLocations()
{
    // Clear obstacle Locations
    addToMap(collectibleLocations, '-');
    // Add collectibles to the map
    numberOfCollectibles = int(numberOfObstacles * 0.15);
    // Generate new set of collectibles as per the updated count
    collectibleLocations = generateCoordinates(distributionRange, numberOfCollectibles);
    // Add the generated collectibles to the map
    addToMap(collectibleLocations, '+');
    // Make sure that holes don't replace start and end points
    addToMap(destinationLocation, '@');
    addToMap(startingLocations, 'a');
}


void World::resetHoleLocations()
{
    // Clear hOLE Locations
    addToMap(holesLocations, '-');
    // update number of holes on the map
    numberOfHoles = int ((level + 1) * 0.1 * numberOfObstacles);
    // Generate new set of hole locations
    holesLocations = generateCoordinates(distributionRange, numberOfHoles);
    // Add generated holes to the map
    addToMap(holesLocations, 'O');
    // Make sure that holes don't replace start and end points
    addToMap(destinationLocation, '@');
    addToMap(startingLocations, 'a');
}


void World::resetObstacleLocations()
{
    // Clear obstacle Locations
    addToMap(obstacleLocations, '-');
    // Generate new set of obstacles as per the updated obstacle count
    generateObstacleLocations();
    // Add the generated obstacles to the map
    addToMap(obstacleLocations, 'X');
    // Make sure that holes don't replace start and end points
    addToMap(destinationLocation, '@');
    addToMap(startingLocations, 'a');
}


void World::generateObstacleLocations()
{
    obstacleLocations = generateCoordinates(distributionRange, numberOfObstacles);
}


bool World:: isValidMove(std::pair<int, int> nextPosition)
{
    // If the next location is an empty space without an obstacle, return true
    if ((worldMap[nextPosition.first][nextPosition.second]).content== '-')
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::pair<int, int> World:: getWorldDimensions()
{
    return worldMapDimensions;
}

bool World:: moveFromTo(std::pair<int, int> currentLocation, std::pair<int, int> nextLocation, char robotID)
{
    bool validateMove = isValidMove(nextLocation);
    if (validateMove)
    {
        // Move the robot to the next location
        (worldMap[nextLocation.first][nextLocation.second]).content = robotID;
        // Empty the previous location from where robot has been displaced
        (worldMap[currentLocation.first][currentLocation.second]).content = '-';
        return true;

    }
    else{
        return false;
    }
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

void World::show(int fieldID)
{
    int length = worldMapDimensions.first;
    int breadth = worldMapDimensions.second;
    // Print

    for (auto &row : worldMap)
    {
        for(auto &col : row)
        {
            switch(fieldID)
            {
                case 0:
                    {
                        std::cout<<" "<<col.content<<" ";
                        break;
                    }
                case 1:
                    {
                        std::cout<<" "<<col.flag<<" ";
                        break;
                    }
                case 2:
                    {
                        std::cout<<"("<<col.previous.first<<", "<<col.previous.second<<")";
                        break;
                    }
                case 3:
                    {
                        std::cout<<"("<<col.next.first<<", "<<col.next.second<<")";
                        break;
                    }
                case 4:
                    {
                        std::cout<<" "<<col.trail<<" ";
                        break;
                    }
            }

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

